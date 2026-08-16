// ============================================================================
// 头文件（include）区
// 作用：把用到的外部库"说明书"引入进来，才能使用里面的功能。
// ============================================================================

// SFML/Graphics.hpp：SFML 图形库的总入口，包含窗口、图形、颜色、形状等。
// 现阶段不需要记住所有 SFML API，只要知道它负责窗口、输入和绘制即可。
#include <SFML/Graphics.hpp>

// std::clamp(值, 最小值, 最大值)：把数值限制在指定区间内。
// 例如 std::clamp(10, 0, 5) 的结果是 5（超出的部分被"截断"）。
#include <algorithm>

// std::sqrt(数)：计算平方根。这里用来求两点之间的距离（勾股定理）。
#include <cmath>

// std::optional：表示"可能有值，也可能没有值"的容器。
// SFML 用它返回窗口事件：有事件时装着事件，没事件时为空。
#include <optional>

// ============================================================================
// 程序入口：main 函数是 C++ 程序的起点，程序从这里开始执行。
// ============================================================================
int main()
{
    // ---- 常量定义区 ------------------------------------------------------
    // 把游戏用到的"固定数值"集中放在开头，之后改参数只改这里。

    // 窗口尺寸：宽 960 像素、高 540 像素。
    // sf::Vector2u = 两个 unsigned int（无符号整数）组成的二维向量。
    // 用 unsigned 是因为像素尺寸不可能是负数，写起来更安全。
    // constexpr = 编译期常量，运行时不能再修改（省内存、更安全）。
    constexpr sf::Vector2u windowSize{960, 540};

    // 英雄尺寸：宽 56、高 56 的方块。
    // sf::Vector2f = 两个 float 组成的二维向量，用小数是因为后面
    // 要参与除法运算（如 heroSize / 2.f），整数除会丢失小数部分。
    constexpr sf::Vector2f heroSize{56.f, 56.f};

    // 英雄移动速度：每秒移动 260 个单位（像素）。
    // 末尾的 .f 表示这是一个 float（单精度小数）类型的字面量。
    constexpr float moveSpeed = 260.f;

    // ---- 窗口创建区 ------------------------------------------------------

    // 创建游戏窗口对象，名字叫 window。
    // 构造函数有 3 个参数，各司其职：
    //   参数1: sf::VideoMode(windowSize) —— 窗口尺寸（960×540）
    //   参数2: "Pocket Heroes..."        —— 窗口标题栏上显示的文字
    //   参数3: sf::Style::Default        —— 窗口外观样式
    //   sf::Style::Default 表示"标准窗口"：有标题栏、可拖动、
    //   有关闭按钮、有最大化按钮、还能用鼠标拖动调整窗口大小。
    // 之后所有绘制、事件处理都通过 window 这个对象进行。
    sf::RenderWindow window(
        sf::VideoMode(windowSize),
        "Pocket Heroes - Click Hero, Then Click Destination",
        sf::Style::Default);

    // 限制最高帧率为每秒 120 帧。
    // 若不限制，游戏会以电脑能跑多快就跑多快的方式运行，
    // 空闲时白白占用大量 CPU/GPU 资源。
    window.setFramerateLimit(120);

    // ---- 英雄创建与外观配置区 ---------------------------------------------

    // 创建英雄对象：sf::RectangleShape（矩形）类，尺寸 56×56。
    // 现阶段用蓝色方块代替英雄，以后可以换成图片。
    // 这一行执行了"构造 + 传参"：括号里的 heroSize 就是矩形的宽高。
    sf::RectangleShape hero(heroSize);

    // ① 设置原点（origin）：决定"位置坐标"指的是图形的哪个点。
    //    默认原点在矩形【左上角】；这里把原点改成矩形【中心】，
    //    之后 setPosition 设置的坐标就代表"英雄中心"在哪。
    //    好处：移动和判断碰撞都按中心算，非常直观。
    hero.setOrigin(heroSize / 2.f);

    // ② 设置初始位置：把英雄中心放到窗口坐标 (180, 270)。
    //    SFML 坐标系：原点 (0,0) 在窗口【左上角】，
    //    x 坐标向右增大，y 坐标向下增大。
    //    窗口是 960×540，所以 (180, 270) 大致在窗口左侧中部。
    hero.setPosition({180.f, 270.f});

    // ③ 设置填充颜色：用 RGB 三个数字混合出颜色，(70,135,230) 是蓝色。
    //    每个数字范围 0~255，表示该颜色分量的强度：
    //    红 70 / 绿 135 / 蓝 230（蓝最多，所以看起来是蓝色）。
    hero.setFillColor(sf::Color(70, 135, 230));

    // ④ 边框（描边）：给方块加一圈 4 像素宽的轮廓，用来做"选中反馈"。
    //    透明色 = 完全看不见。此时英雄还没被选中，所以描边是透明的。
    //    等玩家点击选中英雄后，主循环里会把描边改成金色。
    hero.setOutlineThickness(4.f);
    hero.setOutlineColor(sf::Color::Transparent);

    // ---- 目标点标记创建与外观配置区 ----------------------------------------

    // 目标点：一个半径 9 像素的"空心圆环"，用来标记英雄要去的位置。
    // 注意：它只是视觉提示，不参与任何移动计算。
    // sf::CircleShape 构造函数的参数是圆的半径。
    sf::CircleShape targetMarker(9.f);

    // 原点设到圆心：这样 setPosition 设置的坐标就是圆心的坐标。
    targetMarker.setOrigin({9.f, 9.f});

    // "空心"的实现方法：圆内填充透明色（看不见内部），
    // 只保留 3 像素宽的金色描边 (245,195,70)，
    // 看起来就是一个金色的圆环标记。
    targetMarker.setFillColor(sf::Color::Transparent);
    targetMarker.setOutlineThickness(3.f);
    targetMarker.setOutlineColor(sf::Color(245, 195, 70));

    // ---- 游戏状态变量区 -----------------------------------------------------

    // targetPosition：记录"英雄当前要前往的目标坐标"（两个 float）。
    // 初始值直接取英雄当前的位置，所以游戏刚开始时
    // "英雄所在位置 == 目标位置"，两者距离为 0，英雄自然一动不动。
    // 玩家选中英雄并点击空地时，这个值才会被更新成新的目标点。
    sf::Vector2f targetPosition = hero.getPosition();

    // bool（布尔）类型只有两个取值：true（真）和 false（假），
    // 非常适合记录"是否选中"和"是否显示"这类只有两种状态的事情。
    bool heroSelected = false;  // 玩家是否已经选中英雄？
    bool targetVisible = false; // 金色的目标圆环是否显示？

    // Clock（时钟）：用来测量每一帧经过了多长时间。
    // 有了它才能计算 deltaTime，让移动速度与帧率无关。
    sf::Clock clock;

    // ==========================================================================
    // 游戏主循环
    // 只要窗口还开着，就不断重复做三件事：
    //   1. 处理输入（鼠标点击等）   2. 更新游戏状态（英雄移动）
    //   3. 绘制画面（清空 → 画 → 显示）
    // 每一轮循环 = 一帧画面。
    // ==========================================================================
    while (window.isOpen())
    {
        // ---- 第 1 步：处理输入事件 ------------------------------------------

        // 一个画面帧内可能产生多个事件（比如连续点了好几下鼠标），
        // 所以用 while 循环把这一帧积累的所有事件全部处理完。
        // window.pollEvent() 从事件队列里取一个事件：
        //   有事件时返回装着事件的 optional（循环继续）；
        //   没事件时返回空的 optional（循环结束）。
        // 事件例子：关闭窗口、按下鼠标、松开鼠标、按键等。
        while (const std::optional event = window.pollEvent())
        {
            // event->is<sf::Event::Closed>()：判断这个事件是不是"关闭窗口"。
            // is<T>() 是"这个事件是不是 T 类型？"的判断方法。
            // 如果是，就调用 window.close() 关闭窗口，
            // 外层 while (window.isOpen()) 判断为假，游戏循环自然结束。
            if (event->is<sf::Event::Closed>())
            {
                window.close();
            }

            // getIf<T>()：如果事件是 T 类型，返回指向它的指针；否则返回 nullptr。
            // 这句的写法"声明变量 + 判断非空"一步完成：
            // 只有事件确实是"鼠标按键按下"时，mousePressed 才有效，
            // 并且变量只在这个 if 内部存在（作用域限制）。
            if (const auto* mousePressed = event->getIf<sf::Event::MouseButtonPressed>())
            {
                // 当前游戏只响应鼠标左键，按其他键（右键、中键）直接跳过。
                // mousePressed->button 记录的是按下的哪个键。
                // continue 跳过本次循环的剩余部分，处理下一个事件。
                if (mousePressed->button != sf::Mouse::Button::Left)
                {
                    continue;
                }

                // 鼠标事件里给的是"窗口像素坐标"（整数，左上角为原点）。
                // mapPixelToCoords 把它转换成"游戏世界坐标"。
                // 目前窗口没有滚动/缩放，两者数值一样，
                // 但写成规范形式，以后加了镜头移动也不会出错。
                const sf::Vector2f mousePosition = window.mapPixelToCoords(mousePressed->position);

                // getGlobalBounds() 返回英雄在世界坐标中的"包围矩形"（位置+尺寸）。
                // contains(点) 判断这个点是否落在矩形内部。
                // 整个条件 = "鼠标点击的位置是否点到了英雄？"
                if (hero.getGlobalBounds().contains(mousePosition))
                {
                    // 点到了英雄 → 标记为已选中，
                    // 并把英雄描边改成金色，给玩家"选中了"的视觉反馈。
                    heroSelected = true;
                    hero.setOutlineColor(sf::Color(245, 195, 70));
                }
                else if (heroSelected)
                {
                    // 没点到英雄，但【之前已经选中过英雄】→ 点击空地设置目标点。
                    // 注意逻辑顺序：必须先选中英雄，再点空地，才会移动。

                    // 计算英雄宽、高的一半，用于下面的边界限制。
                    const float halfWidth = heroSize.x / 2.f;
                    const float halfHeight = heroSize.y / 2.f;

                    // std::clamp(值, 最小, 最大)：把值限制在 [最小, 最大] 区间。
                    // 限制目标坐标不能太靠近窗口边缘（至少留出半个英雄身位），
                    // 这样英雄移动过去后不会有一部分跑到窗口外面。
                    // 注意 windowSize 是 unsigned int，而坐标是 float，
                    // 所以要用 static_cast<float> 先把窗口尺寸转成 float 再相减。
                    targetPosition.x = std::clamp(mousePosition.x, halfWidth, static_cast<float>(windowSize.x) - halfWidth);
                    targetPosition.y = std::clamp(mousePosition.y, halfHeight, static_cast<float>(windowSize.y) - halfHeight);

                    // 把金色圆环移到新的目标位置，并让它显示出来。
                    targetMarker.setPosition(targetPosition);
                    targetVisible = true;
                }
            }
        }

        // ---- 第 2 步：更新游戏状态（让英雄朝目标移动） ----------------------

        // clock.restart()：返回"上次调用到现在经过的时间"，并把计时器重新归零。
        // asSeconds() 把时间转成以"秒"为单位的 float。
        // 例如 60 FPS 时，每帧约 1/60 ≈ 0.0167 秒。
        const float deltaTime = clock.restart().asSeconds();

        // 向量相减：目标位置 − 英雄当前位置 = "从英雄指向目标"的偏移向量。
        // 这个向量既有方向（指向目标）又有大小（距离有多远）。
        const sf::Vector2f offset = targetPosition - hero.getPosition();

        // 用勾股定理计算偏移向量的长度（√(x² + y²)），
        // 也就是"英雄当前位置到目标点的直线距离"。
        const float distance = std::sqrt(offset.x * offset.x + offset.y * offset.y);

        // 这一帧英雄能移动的距离 = 每秒速度 × 这一帧的秒数。
        // 之所以乘 deltaTime：不管电脑帧率高（200 FPS）还是低（30 FPS），
        // 每帧移动的量不同，但每秒钟移动的总距离都恒定为 260，速度保持一致。
        const float movement = moveSpeed * deltaTime;

        // 只有当距离 > 0 时才需要移动。
        // 距离为 0 时下面会执行 offset / distance（除以 0），
        // 除数为 0 在数学上没有意义，会导致程序崩溃，所以要先判断。
        if (distance > 0.f)
        {
            // 情况 A：剩余距离 ≤ 本帧能走的距离 → 一步到位，直接到达目标点。
            // 这样避免英雄在目标点附近"来回抖动"（每帧越过一点又折返）。
            // 到达后隐藏金色圆环，因为目的地已经到达了。
            if (distance <= movement)
            {
                hero.setPosition(targetPosition);
                targetVisible = false;
            }
            else // 情况 B：距离还远，本帧走不完 → 朝目标走一段。
            {
                // offset / distance：把偏移向量除以它的长度，
                // 得到一个"长度为 1、方向仍指向目标"的向量，这叫"单位化"。
                // 单位向量 × movement = 本帧朝目标方向移动的具体位移。
                // hero.move(位移) 在英雄当前位置基础上加上这个位移。
                hero.move(offset / distance * movement);
            }
        }

        // ---- 第 3 步：绘制画面 ----------------------------------------------

        // 每帧都按"清空 → 绘制 → 显示"三步生成画面：
        // 清空：把上一帧的内容全部擦掉，并涂上背景色 (30,35,45)（深蓝灰）。
        // 如果不先清空，新一帧会和上一帧叠在一起变成残影。
        window.clear(sf::Color(30, 35, 45));

        // 只有目标点可见时才绘制它（点空地前是隐藏的）。
        if (targetVisible)
        {
            window.draw(targetMarker);
        }

        // 绘制英雄方块。注意绘制顺序：先画目标圈，再画英雄，
        // 后画的会盖在先画的上面（英雄在圆环上方，不遮挡关系混乱）。
        window.draw(hero);

        // display：把这一帧绘制好的内容一次性送到屏幕上。
        // 所有 draw 只是"画到后台缓冲"，display 才真正"显示出来"。
        window.display();
    }
}
