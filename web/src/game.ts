import type { MatComModule } from "./wasm";

export interface GameSnapshot {
  state: number;
  score: number;
  high_score: number;
  lives: number;
  current_level: number;
  ship_x: number;
  ship_y: number;
  projectile_count: number;
  enemy_count: number;
  enemies_destroyed: number;
  survival_timer: number;

  mode: number;
  enemy_x: Int32Array;
  enemy_y: Int32Array;
  enemy_active: Int32Array;
  enemy_type: Int32Array;
  enemy_life: Int32Array;
  projectile_x: Int32Array;
  projectile_y: Int32Array;
  projectile_active: Int32Array;
}

const MAX_ENEMIES = 10;
const MAX_PROJECTILES = 15;

export const SNAPSHOT_FIELDS =
  11 + // base scalar fields (state..survival_timer, see order in game_state_snapshot_t)
  1 + // mode
  MAX_ENEMIES * 5 + // enemy x/y/active/type/life
  MAX_PROJECTILES * 3; // projectile x/y/active

export const GameState = {
  MENU: 0,
  PLAYING: 1,
  PAUSED: 2,
  GAME_OVER: 3,
  VICTORY: 4,
} as const;

export const GameMode = {
  PROGRESSIVE: 0,
  ALTERNATE: 1,
  RANDOM: 2,
  WAVES: 3,
  FORMATIONS: 4,
} as const;

export class Game {
  private module!: MatComModule;
  private ptr = 0;

  async init(): Promise<void> {
    if (!window.createMatComModule) {
      throw new Error("WASM module factory not found");
    }
    this.module = await window.createMatComModule();
    this.ptr = this.module._game_create(12345);
  }

  setMode(mode: number): void {
    this.module._game_set_mode(this.ptr, mode);
  }

  start(): void {
    this.module._game_start(this.ptr);
  }

  update(dt: number): void {
    this.module._game_update(this.ptr, dt);
  }

  setShip(x: number, y: number): void {
    this.module._game_set_ship(this.ptr, x, y);
  }

  fire(): void {
    this.module._game_fire(this.ptr);
  }

  pause(): void {
    this.module._game_pause(this.ptr);
  }

  resume(): void {
    this.module._game_resume(this.ptr);
  }

  getState(): GameSnapshot {
    const size = SNAPSHOT_FIELDS * 4;
    const buf = this.module._malloc(size);
    try {
      this.module._game_get_state(this.ptr, buf);
      const view = new DataView(this.module.HEAPU8.buffer, buf, size);

      const enemy_x = new Int32Array(MAX_ENEMIES);
      const enemy_y = new Int32Array(MAX_ENEMIES);
      const enemy_active = new Int32Array(MAX_ENEMIES);
      const enemy_type = new Int32Array(MAX_ENEMIES);
      const enemy_life = new Int32Array(MAX_ENEMIES);
      const projectile_x = new Int32Array(MAX_PROJECTILES);
      const projectile_y = new Int32Array(MAX_PROJECTILES);
      const projectile_active = new Int32Array(MAX_PROJECTILES);

      let o = 11; // after the base scalars (incl. survival_timer at index 10)
      const mode = view.getInt32(o * 4, true);
      o++;
      for (let i = 0; i < MAX_ENEMIES; i++, o++) enemy_x[i] = view.getInt32(o * 4, true);
      for (let i = 0; i < MAX_ENEMIES; i++, o++) enemy_y[i] = view.getInt32(o * 4, true);
      for (let i = 0; i < MAX_ENEMIES; i++, o++) enemy_active[i] = view.getInt32(o * 4, true);
      for (let i = 0; i < MAX_ENEMIES; i++, o++) enemy_type[i] = view.getInt32(o * 4, true);
      for (let i = 0; i < MAX_ENEMIES; i++, o++) enemy_life[i] = view.getInt32(o * 4, true);
      for (let i = 0; i < MAX_PROJECTILES; i++, o++) projectile_x[i] = view.getInt32(o * 4, true);
      for (let i = 0; i < MAX_PROJECTILES; i++, o++) projectile_y[i] = view.getInt32(o * 4, true);
      for (let i = 0; i < MAX_PROJECTILES; i++, o++) projectile_active[i] = view.getInt32(o * 4, true);

      return {
        state: view.getInt32(0, true),
        score: view.getInt32(4, true),
        high_score: view.getInt32(8, true),
        lives: view.getInt32(12, true),
        current_level: view.getInt32(16, true),
        ship_x: view.getInt32(20, true),
        ship_y: view.getInt32(24, true),
        projectile_count: view.getInt32(28, true),
        enemy_count: view.getInt32(32, true),
        enemies_destroyed: view.getInt32(36, true),
        survival_timer: view.getFloat32(40, true),
        mode,
        enemy_x,
        enemy_y,
        enemy_active,
        enemy_type,
        enemy_life,
        projectile_x,
        projectile_y,
        projectile_active,
      };
    } finally {
      this.module._free(buf);
    }
  }

  destroy(): void {
    if (this.ptr) {
      this.module._game_destroy(this.ptr);
      this.ptr = 0;
    }
  }
}
