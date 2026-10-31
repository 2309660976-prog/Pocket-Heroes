"""只读测量章节路口，输出预览连接层所需坐标与颜色；不修改图片。"""
import json
from pathlib import Path
from statistics import median

from PIL import Image


def rgb(colors):
    return '#' + ''.join(f'{int(median(c[i] for c in colors)):02x}' for i in range(3))


def mouth(image, x, target=None):
    """从画面中部寻找最长的土黄色连续区域，避免把草地和积雪误认为路。"""
    w, h = image.size
    runs, start = [], None
    for y in range(int(h * .23), int(h * .78)):
        r, g, b = image.getpixel((x, y))[:3]
        is_road = r > 145 and g > 100 and r - g > 17 and g - b > 17
        if is_road and start is None:
            start = y
        if not is_road and start is not None:
            if y - start >= 12:
                runs.append((start, y - 1))
            start = None
    if start is not None:
        runs.append((start, int(h * .78) - 1))
    if not runs:
        raise ValueError(f'路口未识别: x={x}')
    a, b = min(runs, key=lambda ab: abs(sum(ab) / 2 - (target or h * .5)))
    # 深色轮廓通常紧贴色块，统一计入宽度，保证过渡段不压窄路口。
    top, bottom = a, b
    for y in range(a - 1, max(a - 10, 0), -1):
        r, g, _ = image.getpixel((x, y))[:3]
        if r < 130 and g < 110:
            top = y
        else:
            break
    for y in range(b + 1, min(b + 10, h)):
        r, g, _ = image.getpixel((x, y))[:3]
        if r < 130 and g < 110:
            bottom = y
        else:
            break
    middle = (top + bottom) / 2
    fill = rgb([image.getpixel((x, y))[:3] for y in range(int(a + (b-a)*.3), max(int(a+(b-a)*.7), a+1))])
    return dict(y=round(middle / h, 6), width=round((bottom - top + 1) / h, 6), fill=fill)


root = Path(__file__).resolve().parents[1]
result = []
for path in sorted((root / 'src/assets/maps').glob('chapter-??-*.png')):
    image = Image.open(path).convert('RGB')
    w, h = image.size
    record = dict(file=path.name, size=[w, h])
    for side, x, inner in [('left', 3, 43), ('right', w-4, w-44)]:
        edge = mouth(image, x)
        inside = mouth(image, inner, edge['y'] * h)
        # 统一使用横向前进方向的切线斜率，不要求所有路口水平。
        edge['slope'] = round((inside['y'] - edge['y']) * h / (inner-x), 4)
        edge['ground'] = [rgb([image.getpixel((x, max(0, min(h-1, int(h*t)+dy))))[:3] for dy in range(-2, 3)]) for t in [.05,.2,.35,.65,.8,.95]]
        record[side] = edge
    result.append(record)
print(json.dumps(result, ensure_ascii=False))
