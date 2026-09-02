export interface MatComModule {
  _game_create(seed: number): number;
  _game_set_mode(ptr: number, mode: number): void;
  _game_start(ptr: number): void;
  _game_update(ptr: number, dt: number): void;
  _game_set_ship(ptr: number, x: number, y: number): void;
  _game_fire(ptr: number): void;
  _game_get_state(ptr: number, out: number): void;
  _game_destroy(ptr: number): void;
  _malloc(size: number): number;
  _free(ptr: number): void;
  HEAPU8: Uint8Array;
  HEAPU32: Uint32Array;
  HEAPF32: Float32Array;
}

export interface MatComModuleFactory {
  (options?: Record<string, unknown>): Promise<MatComModule>;
}

declare global {
  interface Window {
    createMatComModule: MatComModuleFactory;
  }
}
