import { Game, GameState, type GameSnapshot } from "./game";

const canvas = document.getElementById("game-canvas") as HTMLCanvasElement;
const loading = document.getElementById("loading") as HTMLDivElement;
const ctx = canvas.getContext("2d")!;

const game = new Game();

let lastTime = performance.now();
let running = false;
let state: number = GameState.MENU;

function renderShip(x: number, y: number): void {
  ctx.fillStyle = "#00e5ff";
  ctx.fillRect(x - 20, y - 12, 40, 24);
}

function renderHud(snap: GameSnapshot): void {
  ctx.fillStyle = "#ffffff";
  ctx.font = "24px sans-serif";
  ctx.fillText(`Score: ${snap.score}`, 20, 40);
  ctx.fillText(`Lives: ${snap.lives}`, 200, 40);
  ctx.fillText(`Level: ${snap.current_level}`, 340, 40);
}

function frame(now: number): void {
  const dt = Math.min((now - lastTime) / 1000, 0.1);
  lastTime = now;

  if (running) {
    game.update(dt);
  }

  const snap = game.getState();
  state = snap.state;

  ctx.clearRect(0, 0, canvas.width, canvas.height);
  renderShip(snap.ship_x, snap.ship_y);
  renderHud(snap);

  if (state !== GameState.PLAYING) {
    ctx.fillStyle = "rgba(0,0,0,0.6)";
    ctx.fillRect(0, 0, canvas.width, canvas.height);
    ctx.fillStyle = "#ffffff";
    ctx.font = "64px sans-serif";
    ctx.textAlign = "center";
    if (state === GameState.MENU) ctx.fillText("MatCom Invasion", canvas.width / 2, canvas.height / 2);
    else if (state === GameState.GAME_OVER) ctx.fillText("Game Over", canvas.width / 2, canvas.height / 2);
    else if (state === GameState.VICTORY) ctx.fillText("Victory!", canvas.width / 2, canvas.height / 2);
    ctx.textAlign = "left";
  }

  requestAnimationFrame(frame);
}

async function init(): Promise<void> {
  try {
    await game.init();
    loading.style.display = "none";
    canvas.style.display = "block";
    running = true;
    requestAnimationFrame(frame);
  } catch (err) {
    loading.textContent = `Failed to load WASM: ${(err as Error).message}`;
  }
}

window.addEventListener("mousemove", (e) => {
  const rect = canvas.getBoundingClientRect();
  const scaleX = canvas.width / rect.width;
  const scaleY = canvas.height / rect.height;
  const x = Math.round((e.clientX - rect.left) * scaleX);
  const y = Math.round((e.clientY - rect.top) * scaleY);
  game.setShip(x, y);
});

window.addEventListener("click", () => {
  if (state === GameState.MENU) {
    game.setMode(0);
    game.start();
    running = true;
  } else if (state === GameState.PLAYING) {
    game.fire();
  }
});

void init();
