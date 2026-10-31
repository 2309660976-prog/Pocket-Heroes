// 用 Edge 的真实页面检查十处连接几何与滚动，并输出当前地图及接缝巡检截图。
const fs = require('fs');
const assert = require('assert/strict');

(async () => {
  const pages = await (await fetch('http://127.0.0.1:9231/json')).json();
  const ws = new WebSocket(pages.find(p => p.type === 'page').webSocketDebuggerUrl);
  await new Promise(resolve => ws.addEventListener('open', resolve, { once: true }));
  let sequence = 0;
  const pending = new Map(), errors = [];
  ws.addEventListener('message', e => {
    const message = JSON.parse(e.data);
    if (message.method === 'Runtime.exceptionThrown') errors.push(message.params);
    if (message.id) { pending.get(message.id)?.(message); pending.delete(message.id); }
  });
  const call = (method, params = {}) => new Promise((resolve, reject) => {
    const id = ++sequence;
    pending.set(id, result => result.error ? reject(Error(JSON.stringify(result.error))) : resolve(result));
    ws.send(JSON.stringify({ id, method, params }));
  });
  const evaluate = async expression => {
    const result = await call('Runtime.evaluate', { expression, returnByValue: true, awaitPromise: true });
    if (result.result.exceptionDetails) throw Error(JSON.stringify(result.result.exceptionDetails));
    return result.result.result.value;
  };
  await call('Runtime.enable');
  await call('Emulation.setDeviceMetricsOverride', { width: 1440, height: 780, deviceScaleFactor: 1, mobile: false });
  await call('Page.navigate', { url: 'file:///E:/Pocket%20Heroes/docs/%E7%AB%A0%E8%8A%82%E8%83%8C%E6%99%AF%E9%A2%84%E8%A7%88.html' });
  for (let retry = 0; retry < 80; retry++) {
    if (await evaluate('!!window.mapSeamState && [...document.images].every(i=>i.complete&&i.naturalWidth>0)')) break;
    await new Promise(resolve => setTimeout(resolve, 100));
  }
  const result = await evaluate(`(()=>{
    const s=window.mapSeamState;
    return {maps:s.maps.length,joins:s.joins.length,images:[...document.images].every(i=>i.complete&&i.naturalWidth>0),
      checked:s.joins.map((j,i)=>{
        const a=s.maps[i].right,b=s.maps[i+1].left,scale=600/s.maps[i].size[1];
        return {from:j.from,to:j.to,errorA:Math.abs(j.ay-(a.y*600-a.slope*(40-3*scale))),errorB:Math.abs(j.by-(b.y*600+b.slope*(40-3*scale))),widthA:j.widthA,widthB:j.widthB};
      })};})()`);
  assert.equal(result.maps, 11); assert.equal(result.joins, 10); assert(result.images);
  result.checked.forEach(j => { assert(j.errorA < .1 && j.errorB < .1); assert(j.widthA > 10 && j.widthB > 10); });
  // 真实输入检查，而不是只调用页面自己的滚动函数。
  await call('Input.dispatchMouseEvent', { type: 'mouseWheel', x: 700, y: 400, deltaY: 500, deltaX: 0 });
  await new Promise(resolve => setTimeout(resolve, 100));
  result.wheel = await evaluate('document.getElementById("viewport").scrollLeft'); assert(result.wheel >= 400);
  await evaluate('document.getElementById("viewport").scrollLeft=0');
  await call('Input.dispatchMouseEvent', { type: 'mousePressed', x: 1000, y: 600, button: 'left', clickCount: 1 });
  await call('Input.dispatchMouseEvent', { type: 'mouseMoved', x: 700, y: 600, button: 'left', buttons: 1 });
  await call('Input.dispatchMouseEvent', { type: 'mouseReleased', x: 700, y: 600, button: 'left', clickCount: 1 });
  result.drag = await evaluate('document.getElementById("viewport").scrollLeft'); assert(result.drag >= 250);
  await evaluate('document.getElementById("jump").value="10";document.getElementById("jump").dispatchEvent(new Event("change"))');
  await new Promise(resolve => setTimeout(resolve, 1200));
  result.lastChapter = await evaluate('document.getElementById("viewport").scrollLeft'); assert(result.lastChapter > 12500);
  await evaluate('document.getElementById("viewport").scrollLeft=0;document.getElementById("jump").value="0"');
  let shot = await call('Page.captureScreenshot', { format: 'png' });
  fs.writeFileSync('src/assets/maps/chapter-background-preview.png', Buffer.from(shot.result.data, 'base64'));
  await evaluate('document.getElementById("review").click()');
  await call('Emulation.setDeviceMetricsOverride', { width: 1440, height: 1600, deviceScaleFactor: 1, mobile: false });
  await new Promise(resolve => setTimeout(resolve, 150));
  assert.equal(await evaluate('document.querySelectorAll(".card").length'), 10);
  shot = await call('Page.captureScreenshot', { format: 'png' });
  fs.writeFileSync('src/assets/maps/chapter-seams-preview.png', Buffer.from(shot.result.data, 'base64'));
  assert.equal(errors.length, 0);
  console.log(JSON.stringify(result));
  await call('Browser.close'); ws.close();
})().catch(error => { console.error(error); process.exit(1); });
