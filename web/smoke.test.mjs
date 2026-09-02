import { chromium } from "playwright";

const BASE = "http://localhost:4173";

const errs = [];
const browser = await chromium.launch();
const page = await browser.newPage();
page.on("console", (m) => {
  if (m.type() === "error") errs.push(m.text());
});
page.on("pageerror", (e) => errs.push(String(e)));

await page.goto(BASE, { waitUntil: "networkidle" });
await page.waitForSelector(".mode-card", { timeout: 15000 });
console.log("boot + menu OK");

// game starts asynchronously after wasm init; wait until __start is present
await page.waitForFunction(() => typeof (window).__start === "function", undefined, { timeout: 15000 });

const snap = () => page.evaluate(() => (window).__snap());
const start = (m) => page.evaluate((mode) => (window).__start(mode), m);

const playUntilPlaying = async (m) => {
  await start(m);
  await page.waitForFunction(() => (window).__snap().state === 1, undefined, { timeout: 5000 });
  return snap();
};

const hasTimer = async () => {
  const el = page.locator("#hud-timer");
  const hidden = await el.getAttribute("hidden");
  return { visible: hidden === null, text: (await el.textContent()).trim() };
};

const waitForTimerVisible = async (visible) => {
  await page.waitForFunction(
    (wantVisible) => {
      const el = document.getElementById("hud-timer");
      const isVisible = el && el.getAttribute("hidden") === null;
      return isVisible === wantVisible;
    },
    visible,
    { timeout: 4000 },
  );
};

// ── Progressive: full batch of 6 spawns immediately ──
let s = await playUntilPlaying(0);
if (s.enemy_count !== 6) {
  console.error("FAIL Progressive enemy_count != 6 ->", s.enemy_count);
  process.exit(1);
}
if (s.survival_timer === undefined) {
  console.error("FAIL survival_timer missing from snapshot");
  process.exit(1);
}
console.log("Progressive full batch OK (enemy_count=" + s.enemy_count + ", mode=" + s.mode + ")");

// ── Random: survival countdown timer visible ──
await playUntilPlaying(2);
await waitForTimerVisible(true);
let t = await hasTimer();
console.log("Random timer visible:", t.text);
await page.waitForFunction(
  () => {
    const el = document.getElementById("hud-timer");
    return el && !el.hidden && parseFloat(el.textContent) > 40;
  },
  undefined, { timeout: 4000 },
);
console.log("Random countdown starts high:", await page.locator("#hud-timer").textContent());

// ── Alternate: timer visible too ──
await playUntilPlaying(1);
await waitForTimerVisible(true);
t = await hasTimer();
console.log("Alternate timer visible:", t.text);

// ── Waves / Formations: no timer ──
await playUntilPlaying(3);
await waitForTimerVisible(false);
t = await hasTimer();
console.log("Waves no-timer OK");

await playUntilPlaying(4);
await waitForTimerVisible(false);
t = await hasTimer();
console.log("Formations no-timer OK");

await page.screenshot({ path: "/tmp/opencode/smoke-final.png" });
await browser.close();

if (errs.length) {
  console.error("CONSOLE ERRORS:");
  for (const e of errs) console.error(" -", e);
  process.exit(1);
}
console.log("no console errors");
console.log("SMOKE PASS");
