/* Synthesized audio via the Web Audio API — no audio files, no licensing. */

export type SoundName =
  | "shoot"
  | "explosion"
  | "hit"
  | "gameover"
  | "victory"
  | "click";

export class AudioEngine {
  private ctx: AudioContext | null = null;
  private master: GainNode | null = null;
  private musicGain: GainNode | null = null;
  private sfxGain: GainNode | null = null;

  private musicTimer: number | null = null;
  private musicState: string | null = null;
  private nextNoteTime = 0;
  private step = 0;

  muted = false;

  /** Must be called from a user gesture (browser autoplay policy). */
  async init(): Promise<void> {
    if (this.ctx) return;
    const Ctx = window.AudioContext || (window as unknown as { webkitAudioContext: typeof AudioContext }).webkitAudioContext;
    this.ctx = new Ctx();
    this.master = this.ctx.createGain();
    this.master.gain.value = 0.5;
    this.master.connect(this.ctx.destination);

    this.musicGain = this.ctx.createGain();
    this.musicGain.gain.value = 0.22;
    this.musicGain.connect(this.master);

    this.sfxGain = this.ctx.createGain();
    this.sfxGain.gain.value = 0.5;
    this.sfxGain.connect(this.master);
  }

  /** Enable/disable audio entirely. Returns the new muted state. */
  setMuted(m: boolean): boolean {
    this.muted = m;
    if (this.master) this.master.gain.value = m ? 0 : 0.5;
    return m;
  }

  /** (Re)start the music loop for a named state: "menu" | "game" | "intense". */
  playMusic(state: string): void {
    if (!this.ctx || !this.musicGain) return;
    if (this.musicState === state) return;
    this.stopMusic();
    this.musicState = state;
    this.step = 0;
    this.nextNoteTime = this.ctx.currentTime + 0.05;
    this.musicTimer = window.setInterval(() => this.scheduleMusic(), 120);
  }

  stopMusic(): void {
    if (this.musicTimer !== null) {
      window.clearInterval(this.musicTimer);
      this.musicTimer = null;
    }
    this.musicState = null;
  }

  private scheduleMusic(): void {
    if (!this.ctx || !this.musicGain || !this.musicState) return;
    while (this.nextNoteTime < this.ctx.currentTime + 0.25) {
      this.playStep(this.musicState, this.step);
      this.step = (this.step + 1) % 16;
      this.nextNoteTime += this.stepDur();
    }
  }

  private stepDur(): number {
    // ~130 BPM -> eighth notes
    return 60 / 130 / 2;
  }

  private playStep(state: string, s: number): void {
    if (!this.ctx || !this.musicGain) return;
    const t = this.nextNoteTime;

    // droney bass pulse on beats
    if (s % 4 === 0) this.note(t, this.pitchFor(state, 0), "triangle", 0.35);

    // sparse arpeggio
    if (s % 2 === 0 || state === "intense") {
      const triad = [0, 3, 7, 12];
      const idx = Math.floor(s / 2) % triad.length;
      this.note(t, this.pitchFor(state, triad[idx]), "square", 0.2, 0.06);
    }
  }

  private pitchFor(state: string, semis: number): number {
    const base = 220; // A3
    const scale = state === "intense" ? 18 : state === "game" ? 12 : 5;
    return base * Math.pow(2, (scale + semis) / 12);
  }

  private note(
    when: number,
    freq: number,
    type: OscillatorType,
    dur: number,
    gain = 0.16
  ): void {
    if (!this.ctx || !this.musicGain) return;
    const osc = this.ctx.createOscillator();
    const g = this.ctx.createGain();
    osc.type = type;
    osc.frequency.value = freq;
    g.gain.setValueAtTime(0.0001, when);
    g.gain.exponentialRampToValueAtTime(gain, when + 0.01);
    g.gain.exponentialRampToValueAtTime(0.0001, when + dur);
    osc.connect(g);
    g.connect(this.musicGain);
    osc.start(when);
    osc.stop(when + dur + 0.02);
  }

  play(name: SoundName): void {
    if (!this.ctx || !this.sfxGain || this.muted) return;
    const t = this.ctx.currentTime;
    switch (name) {
      case "shoot": {
        const osc = this.ctx.createOscillator();
        const g = this.ctx.createGain();
        osc.type = "sawtooth";
        osc.frequency.setValueAtTime(880, t);
        osc.frequency.exponentialRampToValueAtTime(220, t + 0.08);
        g.gain.setValueAtTime(0.12, t);
        g.gain.exponentialRampToValueAtTime(0.0001, t + 0.1);
        osc.connect(g);
        g.connect(this.sfxGain);
        osc.start(t);
        osc.stop(t + 0.12);
        break;
      }
      case "explosion": {
        this.noiseBurst(t, 0.4, 0.5, 320);
        break;
      }
      case "hit": {
        this.noiseBurst(t, 0.16, 0.2, 900);
        break;
      }
      case "gameover": {
        this.toneSeq(t, [392, 330, 262, 196], 0.22, "sawtooth");
        break;
      }
      case "victory": {
        this.toneSeq(t, [523, 659, 784, 1046], 0.18, "square");
        break;
      }
      case "click": {
        this.toneSeq(t, [660], 0.05, "sine");
        break;
      }
    }
  }

  private noiseBurst(when: number, dur: number, vol: number, cutoff: number): void {
    if (!this.ctx || !this.sfxGain) return;
    const len = Math.floor(this.ctx.sampleRate * dur);
    const buf = this.ctx.createBuffer(1, len, this.ctx.sampleRate);
    const data = buf.getChannelData(0);
    for (let i = 0; i < len; i++) data[i] = (Math.random() * 2 - 1) * (1 - i / len);
    const src = this.ctx.createBufferSource();
    src.buffer = buf;
    const filter = this.ctx.createBiquadFilter();
    filter.type = "lowpass";
    filter.frequency.value = cutoff;
    const g = this.ctx.createGain();
    g.gain.setValueAtTime(vol, when);
    g.gain.exponentialRampToValueAtTime(0.0001, when + dur);
    src.connect(filter);
    filter.connect(g);
    g.connect(this.sfxGain);
    src.start(when);
  }

  private toneSeq(when: number, freqs: number[], stepDur: number, type: OscillatorType): void {
    freqs.forEach((f, i) => {
      if (!this.ctx || !this.sfxGain) return;
      const t = when + i * stepDur;
      const osc = this.ctx.createOscillator();
      const g = this.ctx.createGain();
      osc.type = type;
      osc.frequency.value = f;
      g.gain.setValueAtTime(0.16, t);
      g.gain.exponentialRampToValueAtTime(0.0001, t + stepDur * 0.9);
      osc.connect(g);
      g.connect(this.sfxGain);
      osc.start(t);
      osc.stop(t + stepDur);
    });
  }

  dispose(): void {
    this.stopMusic();
    if (this.ctx) {
      void this.ctx.close();
      this.ctx = null;
    }
  }
}
