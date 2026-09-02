/* Synthesized audio via the Web Audio API — no audio files, no licensing.
   Ambient, evolving music: a low drone + slow pad chords, with a melodic
   arpeggio layered in during gameplay. Long 64-step cycle (not a 3s loop). */

export type SoundName =
  | "shoot"
  | "explosion"
  | "hit"
  | "gameover"
  | "victory"
  | "click";

interface Step {
  state: string;
  step: number;
  when: number;
}

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
    const Ctx =
      window.AudioContext ||
      (window as unknown as { webkitAudioContext: typeof AudioContext }).webkitAudioContext;
    this.ctx = new Ctx();
    this.master = this.ctx.createGain();
    this.master.gain.value = 0.5;
    this.master.connect(this.ctx.destination);

    this.musicGain = this.ctx.createGain();
    this.musicGain.gain.value = 0.15; // ambient, not foreground
    this.musicGain.connect(this.master);

    this.sfxGain = this.ctx.createGain();
    this.sfxGain.gain.value = 0.5;
    this.sfxGain.connect(this.master);
  }

  setMuted(m: boolean): boolean {
    this.muted = m;
    if (this.master) this.master.gain.value = m ? 0 : 0.5;
    return m;
  }

  playMusic(state: string): void {
    if (!this.ctx || !this.musicGain) return;
    if (this.musicState === state) return;
    this.stopMusic();
    this.musicState = state;
    this.step = 0;
    this.nextNoteTime = this.ctx.currentTime + 0.05;
    this.musicTimer = window.setInterval(() => this.scheduleMusic(), 100);
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
    while (this.nextNoteTime < this.ctx.currentTime + 0.2) {
      this.playStep({ state: this.musicState, step: this.step, when: this.nextNoteTime });
      this.step = (this.step + 1) % 64;
      this.nextNoteTime += this.stepDur();
    }
  }

  private stepDur(): number {
    // eighth-note grid; faster in intense play
    const bpm = this.musicState === "intense" ? 150 : 104;
    return 60 / bpm / 2;
  }

  /** Four-note chords we cycle through (minor-ish, spacey). */
  private chordRoots(): number[] {
    // offsets in semitones relative to A (220 Hz)
    return [0, -4, -7, -2]; // Am, F, C(maj feel), G
  }

  /** Pentatonic minor scale used by the arpeggio. */
  private pentatonic(): number[] {
    // A minor pentatonic: A C D E G (intervals 0,3,5,7,10)
    return [0, 3, 5, 7, 10];
  }

  private playStep(s: Step): void {
    if (!this.ctx || !this.musicGain) return;
    const { state, step, when } = s;
    const chordIdx = Math.floor(step / 16) % 4;
    const root = this.chordRoots()[chordIdx];
    const inBar = step % 16;

    // bass drone: hold the root, plucked on each bar start (soft)
    if (inBar === 0) {
      this.musicNote(when, this.freq(root), "sine", this.stepDur() * 3.5, 0.16);
    }

    // pad chord: long, slow, airy on every other bar
    if (state !== "menu") {
      if (inBar === 0 || inBar === 8) {
        const third = root + 3;
        const fifth = root + 7;
        this.musicNote(when, this.freq(root + 12), "sine", this.stepDur() * 15, 0.05);
        this.musicNote(when, this.freq(third + 12), "sine", this.stepDur() * 15, 0.04);
        this.musicNote(when, this.freq(fifth + 12), "sine", this.stepDur() * 15, 0.04);
      }
    }

    // melodic arpeggio: only during play/intense; moves up the pentatonic
    if (state === "menu") return;
    const scale = this.pentatonic();
    const isIntense = state === "intense";
    if (step % 2 === 0 || isIntense) {
      const voice = Math.floor(step / 2) % (scale.length * 2);
      const oct = Math.floor(voice / scale.length);
      const note = scale[voice % scale.length] + 12 * oct;
      const type: OscillatorType = isIntense ? "sawtooth" : "triangle";
      const lfo = (chordIdx % 3) === 0 ? 0 : 1;
      this.musicNote(when, this.freq(root + note), type, this.stepDur(), 0.07);
      // occasional sparkle note an octave up for texture
      if (lfo === 1 && step % 4 === 0) {
        this.musicNote(when, this.freq(root + note + 12), "sine", this.stepDur() * 0.9, 0.025);
      }
    }
  }

  private freq(semitoneOffset: number): number {
    const base = 220; // A3
    return base * Math.pow(2, semitoneOffset / 12);
  }

  private musicNote(when: number, freq: number, type: OscillatorType, dur: number, gain: number): void {
    if (!this.ctx || !this.musicGain) return;
    const osc = this.ctx.createOscillator();
    const g = this.ctx.createGain();
    osc.type = type;
    osc.frequency.value = freq;
    // slow attack for pads, faster for plucks
    const attack = dur > 1 ? 0.4 : 0.01;
    g.gain.setValueAtTime(0.0001, when);
    g.gain.exponentialRampToValueAtTime(gain, when + attack);
    g.gain.setValueAtTime(gain, when + Math.max(attack, dur * 0.7));
    g.gain.exponentialRampToValueAtTime(0.0001, when + dur);
    osc.connect(g);
    g.connect(this.musicGain);
    osc.start(when);
    osc.stop(when + dur + 0.05);
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
