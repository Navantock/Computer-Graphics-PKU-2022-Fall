# CG-HW3项目说明

**学号：190001174   姓名：巫鸿飞   院系：化学与分子工程学院**

北京大学图形学课程小作业3代码。

#### 1.使用的数据

1.模型：使用了一个地球模型并根据learnopengl的相关源码实现了模型的载入与纹理映射贴图。

2.粒子：定义了ParticleSystem并实现了一个子类ParticleRing，单个粒子为正四面体形状，启用了可选的纹理映射贴图。

#### 2.粒子系统说明

粒子系统实现了父类ParticleSystem，该类有成员RenderInit()和Draw()函数实现粒子系统绘制，还提供了根据速度和当前位置绘制动态粒子系统的函数UpdateState.

在此基础上继承此类实现了子类ParticleRing生成环绕原点的粒子环，以选定的半径为均值，设置一个参数var作为σ生成对应的分布，从分布中随机取样生成粒子。设定粒子数量为10000，可以在宏定义中修改。其他参数设置参见第5部分UI交互。

#### 3.光照说明

使用光源：一个点光源，可以在空间内任意移动，也可以调整光的颜色。

光照效果：采用Blinn-Phong着色模型，实现漫反射，镜面反光等效果。另外实现了阴影贴图。

#### 4.相机平滑过渡

UI界面中Camera Transition选项卡下，提供了相机平滑过渡的演示功能。

**Save initial camera**: 存储当前状态为相机初始状态

**Save ending camera**: 存储当前状态为相机终止状态

**Run camera transition**: 演示从当前存储的初始状态到终止状态的平滑过渡

在选项卡下可以看到当前相机状态，包括相机位置向量(Position)， 相机注视方向向量(Front), 相机视野大小(fov)。

平滑过渡的实现为对Position与fov进行线性插值，对Front进行四元数插值的旋转。

改变其中一个或多个相机属性以及播放手势动画同时的过渡演示都是合法操作。您可以选定**任何**位置在**任何**时候运行过渡演示。

#### 5.UI交互

设置的UI交互功能如下:

1.Camera Transition 参见第4部分相机平滑过渡

2.Model Movement and Light Source Settings 模型运动和光源设置

Model movement

-Rotate Earth 旋转地球模型，为模仿黄赤交角，设置了23.5°倾斜角的自转轴

Light source settings

-Light source position

通过滑动条可以调节点光源的位置

-Light source color

通过ImGui内置的颜色板调节光源颜色

3.Particle Setting —— Particle Ring Setting

Ring Params: 粒子环生成参数

-Radius 生成的中心半径

-Degree of dispersion 弥散程度，即Gauss分布方差项

-Angular velocity (rad/s) 环运行的角速度，单位为rad/s

Particle color settings: 粒子环颜色设置

-Transparent Mode: 勾选启用透明模式，即粒子会随时间逐渐淡化透明

-Use Texture: 勾选启用对粒子的纹理映射贴图

-Use Transition Color: 勾选启用过渡的颜色模式，中心为白色，向内外扩散随距离增加逐渐变为对应设置的颜色。<!--$$-->**仅在不启用Use Texture时生效**

可以通过滑动条设置使用颜色的RGB值。

#### 6.项目文件结构:

1.在\src目录下：

- **camera.h**, 实现相机

- **light.h**, 实现光源类LightSrc

- **particle.h**, 实现通用粒子系统父类ParticleSystem和一个子类ParticleRing

- mesh.h，实现网格*

- model.h, 实现模型载入*

- shader.h, 实现shader文件的读取与编译*

- draw_ui.h, 设置并绘制ImGui生成的UI界面

- d.vert, d.geom, d.frag, 深度着色器的顶点着色器，几何着色器，片段着色器文件

- f.vert, f.frag, 实现万向阴影贴图的着色器

- main.cpp, 创建opengl窗口并完成绘制

  *_Ref: [JoeyDeVries/LearnOpenGL: Code repository of all OpenGL chapters from the book and its accompanying website https://learnopengl.com (github.com)_

在\third_party目录下:

- 导入了imgui库用以实现ui界面。
- 更新Assimp库至5.2.4以载入obj文件和mtl文件。

在\data目录下:

-从TurboSquid网站下载了免费的模型地球文件和纹理贴图

-从网站上下在了一张图片作为粒子纹理