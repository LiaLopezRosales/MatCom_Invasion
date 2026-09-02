import type { GameSnapshot } from "./game";

const ENEMY_TYPES = 5;

interface TintedSprite {
  img: HTMLCanvasElement;
  w: number;
  h: number;
}

const imageCache = new Map<string, Promise<HTMLImageElement>>();

function loadImage(url: string): Promise<HTMLImageElement> {
  let p = imageCache.get(url);
  if (!p) {
    p = new Promise((resolve, reject) => {
      const img = new Image();
      img.onload = () => resolve(img);
      img.onerror = () => reject(new Error(`failed to load ${url}`));
      img.src = url;
    });
    imageCache.set(url, p);
  }
  return p;
}

/** Tint a source image to a single neon color via an offscreen canvas. */
function tint(source: HTMLImageElement, color: string): TintedSprite {
  const w = source.width;
  const h = source.height;
  const c = document.createElement("canvas");
  c.width = w;
  c.height = h;
  const cx = c.getContext("2d")!;
  cx.drawImage(source, 0, 0);
  cx.globalCompositeOperation = "source-in";
  cx.fillStyle = color;
  cx.fillRect(0, 0, w, h);
  return { img: c, w, h };
}

export class Renderer {
  private ctx: CanvasRenderingContext2D;
  private ready = false;

  private ship: TintedSprite | null = null;
  private enemies: (TintedSprite | null)[] = [];
  private laser: TintedSprite | null = null;

  private stars: { x: number; y: number; speed: number; size: number }[] = [];

  constructor(private canvas: HTMLCanvasElement) {
    this.ctx = canvas.getContext("2d")!;
    for (let i = 0; i < 110; i++) {
      this.stars.push({
        x: Math.random() * canvas.width,
        y: Math.random() * canvas.height,
        speed: 12 + Math.random() * 55,
        size: 1 + Math.random() * 2.2,
      });
    }
  }

  async load(): Promise<void> {
    const [shipRaw, ...enemyRaws] = await Promise.all([
      loadImage("/assets/sprites/ship.png"),
      loadImage("/assets/sprites/enemy_grunt.png"),
      loadImage("/assets/sprites/enemy_tank.png"),
      loadImage("/assets/sprites/enemy_dart.png"),
      loadImage("/assets/sprites/enemy_hover.png"),
      loadImage("/assets/sprites/enemy_swarm.png"),
    ]);
    const laserRaw = await loadImage("/assets/sprites/laser_red.png").catch(() => null);

    const enemyColors = ["#ff3b5c", "#ff2bd6", "#ffb02b", "#9d4dff", "#2bff88"];
    this.ship = tint(shipRaw, "#00e5ff");
    this.enemies = enemyRaws.map((raw, i) => tint(raw, enemyColors[i % enemyColors.length]));
    this.laser = laserRaw ? tint(laserRaw, "#ffffff") : null;
    this.ctx.imageSmoothingEnabled = true;
    this.ready = true;
  }

  draw(snap: GameSnapshot, _tSec: number): void {
    const { ctx, canvas } = this;
    ctx.clearRect(0, 0, canvas.width, canvas.height);
    this.drawBackground();

    if (!this.ready) return;

    // projectiles
    for (let i = 0; i < snap.projectile_active.length; i++) {
      if (!snap.projectile_active[i]) continue;
      if (this.laser) {
        this.drawSprite(this.laser, snap.projectile_x[i], snap.projectile_y[i], 0.5, "#00e5ff");
      } else {
        ctx.fillStyle = "#00e5ff";
        ctx.fillRect(snap.projectile_x[i] - 3, snap.projectile_y[i] - 14, 6, 28);
      }
    }

    // enemies
    for (let i = 0; i < snap.enemy_active.length; i++) {
      if (!snap.enemy_active[i]) continue;
      const type = snap.enemy_type[i] % ENEMY_TYPES;
      const sprite = this.enemies[type];
      const color = this.enemyColor(type);
      if (sprite) {
        this.drawSprite(sprite, snap.enemy_x[i], snap.enemy_y[i], 0.95, color);
        if (snap.enemy_life[i] > 1) this.drawLifeBar(snap.enemy_x[i], snap.enemy_y[i] - 48, type);
      } else {
        ctx.fillStyle = color;
        ctx.beginPath();
        ctx.arc(snap.enemy_x[i], snap.enemy_y[i], 26, 0, Math.PI * 2);
        ctx.fill();
      }
    }

    // ship
    if (this.ship) this.drawSprite(this.ship, snap.ship_x, snap.ship_y, 0.55, "#00e5ff");
  }

  private enemyColor(type: number): string {
    const colors = ["#ff3b5c", "#ff2bd6", "#ffb02b", "#9d4dff", "#2bff88"];
    return colors[type % colors.length];
  }

  private drawLifeBar(x: number, y: number, type: number): void {
    const { ctx } = this;
    ctx.fillStyle = "rgba(0,0,0,0.55)";
    ctx.fillRect(x - 24, y, 48, 6);
    ctx.fillStyle = this.enemyColor(type);
    ctx.fillRect(x - 24, y, 48, 6);
  }

  private drawSprite(sprite: TintedSprite, cx: number, cy: number, scale: number, glow: string): void {
    const { ctx } = this;
    const w = sprite.w * scale;
    const h = sprite.h * scale;
    const x = cx - w / 2;
    const y = cy - h / 2;
    ctx.save();
    ctx.shadowColor = glow;
    ctx.shadowBlur = 22;
    ctx.drawImage(sprite.img, x, y, w, h);
    ctx.restore();
  }

  private drawBackground(): void {
    const { ctx, canvas, stars } = this;
    const g = ctx.createLinearGradient(0, 0, 0, canvas.height);
    g.addColorStop(0, "#0a0a1f");
    g.addColorStop(0.5, "#0d0d2a");
    g.addColorStop(1, "#120626");
    ctx.fillStyle = g;
    ctx.fillRect(0, 0, canvas.width, canvas.height);

    for (const s of stars) {
      s.y += s.speed * 0.005;
      if (s.y > canvas.height) {
        s.y = -2;
        s.x = Math.random() * canvas.width;
      }
      ctx.fillStyle = `rgba(180,220,255,${(0.3 + (s.size - 1) * 0.25).toFixed(2)})`;
      ctx.fillRect(s.x, s.y, s.size, s.size);
    }
  }
}
