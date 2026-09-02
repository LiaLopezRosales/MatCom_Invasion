import { Game, GameState, GameMode, type GameSnapshot } from "./game";
import { Renderer } from "./render";
import { AudioEngine } from "./audio";
import "./styles.css";

const MODE_META: Record<number, { name: string; win: string; desc: string }> = {
  [GameMode.PROGRESSIVE]: { name: "Progressive", win: "Reach 500 pts", desc: "The weak come first. Difficulty ramps up each wave." },
  [GameMode.ALTERNATE]: { name: "Alternate", win: "10 kills in 30s", desc: "All enemy types mix together. Kill fast, kill smart." },
  [GameMode.RANDOM]: { name: "Random", win: "Survive 45s", desc: "Random hordes from all types. Just stay alive." },
  [GameMode.WAVES]: { name: "Waves", win: "Clear 5 waves", desc: "Enemies arrive in bursts. One wave at a time." },
  [GameMode.FORMATIONS]: { name: "Formations", win: "Reach level 6", desc: "Structured squadrons descend. Break them apart." },
};

const ENEMY_CAP = 10;
const PROJECTILE_CAP = 15;
const INITIAL_LIVES = 3;

const WIN_RANDOM_SURVIVAL = 45;
const WIN_ALTERNATE_TIME = 30;

const $ = <T extends HTMLElement>(id: string): T => document.getElementById(id) as T;

// ── high-score persistence (per-mode, localStorage) ──
const HS_KEY = "matcom_highscores";
function loadHighScores(): Record<number, number> {
  try { return JSON.parse(localStorage.getItem(HS_KEY) || "{}"); } catch { return {}; }
}
function saveHighScore(mode: number, score: number): void {
  const hs = loadHighScores();
  if (!hs[mode] || score > hs[mode]) {
    hs[mode] = score;
    try { localStorage.setItem(HS_KEY, JSON.stringify(hs)); } catch { /* quota */ }
  }
}
function getHighScore(mode: number): number {
  return loadHighScores()[mode] || 0;
}

function blankSnapshot(): GameSnapshot {
  return {
    state: GameState.MENU, score: 0, high_score: 0, lives: 3, current_level: 1,
    ship_x: 0, ship_y: 0, projectile_count: 0, enemy_count: 0, enemies_destroyed: 0,
    survival_timer: 0, mode: 0, enemy_x: new Int32Array(ENEMY_CAP), enemy_y: new Int32Array(ENEMY_CAP),
    enemy_active: new Int32Array(ENEMY_CAP), enemy_type: new Int32Array(ENEMY_CAP),
    enemy_life: new Int32Array(ENEMY_CAP),
    projectile_x: new Int32Array(PROJECTILE_CAP), projectile_y: new Int32Array(PROJECTILE_CAP),
    projectile_active: new Int32Array(PROJECTILE_CAP),
  };
}

async function main(): Promise<void> {
  const canvas = $<HTMLCanvasElement>("game-canvas");
  const shell = $<HTMLDivElement>("shell");
  const loading = $<HTMLDivElement>("loading");
  const loadingError = $<HTMLDivElement>("loading-error");

  const game = new Game();
  const renderer = new Renderer(canvas);
  const audio = new AudioEngine();

  const hudLives = $<HTMLSpanElement>("hud-lives");
  const hudScore = $<HTMLSpanElement>("hud-score");
  const hudLevel = $<HTMLSpanElement>("hud-level");
  const hudMode = $<HTMLSpanElement>("hud-mode");
  const hudHigh = $<HTMLSpanElement>("hud-high");
  const hudTimer = $<HTMLSpanElement>("hud-timer");

  const menuMode = $<HTMLDivElement>("menu-mode");
  const menuHowto = $<HTMLDivElement>("menu-howto");
  const menuPause = $<HTMLDivElement>("menu-pause");
  const menuGameOver = $<HTMLDivElement>("menu-gameover");
  const menuVictory = $<HTMLDivElement>("menu-victory");
  const modeCards = $<HTMLDivElement>("mode-cards");
  const goScore = $<HTMLSpanElement>("go-score");
  const goBest = $<HTMLSpanElement>("go-best");
  const vicScore = $<HTMLSpanElement>("vic-score");
  const vicBest = $<HTMLSpanElement>("vic-best");

  const allMenus = [menuMode, menuHowto, menuPause, menuGameOver, menuVictory];
  const showMenu = (m: HTMLDivElement | null): void => {
    allMenus.forEach((x) => x.classList.remove("active"));
    if (m) m.classList.add("active");
  };

  // ── state tracking ──
  let snap = blankSnapshot();
  let lastState = GameState.MENU;
  let lastLives = 3;
  let running = false;
  let musicKey: string | null = null;
  let lastTime = performance.now();

  // ── mode select cards ──
  function buildModeCards(): void {
    modeCards.innerHTML = "";
    for (const mode of Object.values(GameMode)) {
      const meta = MODE_META[mode];
      if (!meta) continue;
      const card = document.createElement("button");
      card.className = "mode-card";
      card.innerHTML = `<h3>${meta.name}</h3><p>${meta.desc}</p>` +
        `<span class="mode-win">WIN · ${meta.win}</span>`;
      card.addEventListener("click", () => startGame(mode));
      modeCards.appendChild(card);
    }
  }

  function startGame(mode: number): void {
    game.setMode(mode);
    game.start();
    lastLives = INITIAL_LIVES;
    renderer.reset();
    audio.play("click");
    showMenu(null);
  }

  function updateHud(s: GameSnapshot): void {
    hudScore.textContent = String(s.score);
    hudHigh.textContent = String(getHighScore(s.mode));
    hudLevel.textContent = String(s.current_level);
    hudMode.textContent = MODE_META[s.mode]?.name ?? "?";
    hudLives.innerHTML = "";
    for (let i = 0; i < 3; i++) {
      const img = document.createElement("img");
      img.src = "/assets/sprites/ship.png";
      img.className = "hud-life-icon";
      if (i >= s.lives) img.classList.add("lost");
      hudLives.appendChild(img);
    }

    // countdown timer for timed modes
    if (s.mode === GameMode.RANDOM) {
      hudTimer.hidden = false;
      hudTimer.textContent = Math.max(0, Math.ceil(WIN_RANDOM_SURVIVAL - s.survival_timer)) + "s";
    } else if (s.mode === GameMode.ALTERNATE) {
      hudTimer.hidden = false;
      hudTimer.textContent = Math.max(0, Math.ceil(WIN_ALTERNATE_TIME - s.survival_timer)) + "s";
    } else {
      hudTimer.hidden = true;
    }
  }

  function handleStateChange(s: GameSnapshot): void {
    switch (s.state) {
      case GameState.MENU: showMenu(menuMode); setMusic("menu"); break;
      case GameState.PLAYING:
        showMenu(null);
        running = true;
        setMusic(s.mode === GameMode.WAVES ? "intense" : "game");
        break;
      case GameState.PAUSED: showMenu(menuPause); break;
      case GameState.GAME_OVER:
        running = false;
        saveHighScore(s.mode, s.score);
        audio.play("gameover");
        goScore.textContent = String(s.score);
        goBest.textContent = String(getHighScore(s.mode));
        showMenu(menuGameOver);
        setMusic("menu");
        break;
      case GameState.VICTORY:
        running = false;
        saveHighScore(s.mode, s.score);
        audio.play("victory");
        vicScore.textContent = String(s.score);
        vicBest.textContent = String(getHighScore(s.mode));
        showMenu(menuVictory);
        setMusic("menu");
        break;
    }
  }

  function setMusic(key: string): void {
    if (musicKey !== key) {
      musicKey = key;
      audio.playMusic(key);
    }
  }

  function quitToMenu(): void {
    game.setMode(GameMode.PROGRESSIVE);
    game.start();
    running = false;
    audio.play("click");
    showMenu(menuMode);
  }

  // ── main loop ──
  function frame(now: number): void {
    const dt = Math.min((now - lastTime) / 1000, 0.1);
    lastTime = now;

    if (running) game.update(dt);
    tryFire();
    snap = game.getState();
    if (snap.state === GameState.PLAYING && snap.lives < lastLives) {
      renderer.triggerDamageFlash();
    }
    lastLives = snap.lives;
    updateHud(snap);
    renderer.draw(snap, now / 1000);

    if (snap.state !== lastState) {
      lastState = snap.state;
      handleStateChange(snap);
    }
    requestAnimationFrame(frame);
  }

  // ── input: mouse ship + hold to fire ──
  let firing = false;
  let fireAcc = 0;

  const tryFire = (): void => {
    if (snap.state === GameState.PLAYING && firing) {
      if (fireAcc <= 0) {
        game.fire();
        audio.play("shoot");
        fireAcc = 9;
      }
      fireAcc--;
    }
  };

  canvas.addEventListener("mousemove", (e) => {
    const rect = canvas.getBoundingClientRect();
    const x = Math.round(((e.clientX - rect.left) / rect.width) * canvas.width);
    const y = Math.round(((e.clientY - rect.top) / rect.height) * canvas.height);
    game.setShip(x, y);
  });

  canvas.addEventListener("mousedown", (e) => {
    if (e.button !== 0) return;
    firing = true;
    fireAcc = 0;
    tryFire();
  });
  window.addEventListener("mouseup", () => (firing = false));

  window.addEventListener("keydown", (e) => {
    if (e.key === "Escape") {
      if (snap.state === GameState.PLAYING) game.pause();
      else if (snap.state === GameState.PAUSED) game.resume();
    }
  });

  // ── wiring buttons ──
  $<HTMLButtonElement>("btn-howto").addEventListener("click", () => {
    audio.play("click");
    showMenu(menuHowto);
  });
  $<HTMLButtonElement>("btn-back-mode").addEventListener("click", () => {
    audio.play("click");
    showMenu(menuMode);
  });
  $<HTMLButtonElement>("btn-resume").addEventListener("click", () => {
    if (snap.state === GameState.PAUSED) game.resume();
  });
  $<HTMLButtonElement>("btn-quit-pause").addEventListener("click", quitToMenu);

  const restart = (): void => {
    audio.play("click");
    lastLives = INITIAL_LIVES;
    renderer.reset();
    game.start();
  };
  $<HTMLButtonElement>("btn-restart").addEventListener("click", restart);
  $<HTMLButtonElement>("btn-playagain").addEventListener("click", restart);
  $<HTMLButtonElement>("btn-mode-gameover").addEventListener("click", () => {
    audio.play("click");
    showMenu(menuMode);
  });
  $<HTMLButtonElement>("btn-mode-victory").addEventListener("click", () => {
    audio.play("click");
    showMenu(menuMode);
  });

  const btnSound = $<HTMLButtonElement>("btn-sound");
  btnSound.addEventListener("click", () => {
    audio.setMuted(!audio.muted);
    btnSound.textContent = audio.muted ? "✕" : "♪";
  });
  const btnFullscreen = $<HTMLButtonElement>("btn-fullscreen");
  btnFullscreen.addEventListener("click", () => {
    if (document.fullscreenElement) {
      void document.exitFullscreen();
    } else {
      void document.documentElement.requestFullscreen().catch(() => undefined);
    }
  });

  // unlock audio on first user gesture (browser autoplay policy)
  const unlock = (): void => {
    void audio.init().then(() => setMusic("menu"));
    window.removeEventListener("pointerdown", unlock);
    window.removeEventListener("keydown", unlock);
  };
  window.addEventListener("pointerdown", unlock);
  window.addEventListener("keydown", unlock);

  // ── boot ──
  renderer
    .load()
    .then(async () => {
      await game.init();
      buildModeCards();
      loading.style.display = "none";
      shell.hidden = false;
      requestAnimationFrame(frame);
      // small test hooks so headless checks can inspect live game state & start modes
      (window as unknown as { __snap?: () => GameSnapshot; __start?: (m: number) => void }).__snap = () => game.getState();
      (window as unknown as { __start?: (m: number) => void }).__start = (m: number) => startGame(m);
    })
    .catch((err) => {
      loadingError.textContent = `Failed to start: ${(err as Error).message}`;
      loadingError.hidden = false;
    });
}

void main();
