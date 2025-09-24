/*
 * Copyright (C) 2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the 'License');
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an 'AS IS' BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef LOTTIETURBO_LOTTIEHANDLE_H
#define LOTTIETURBO_LOTTIEHANDLE_H

#include "cangjie/animator.h"
#include "dataManager/data_manager.h"
#include "dataManager/data_parse.h"
#include "cangjie/lottie_config_data_bridge.h"
#include "cangjie/lottie_listener.h"
#include <arkui/native_node.h>
#include <chrono>
#include <cstdint>

#include "cangjie/cj_animator.h"

namespace LottieTurbo {

struct LottieSize {
    int32_t width;
    int32_t height;
};

enum class Direction {
    FORWARD = 1, // 正向播放
    REVERSE = -1 // 反向播放
};

constexpr float INVALID_FRAME = -1.0; // 无效帧

//class LottieControllerBridge;

class LottieHandler {
public:
    const int lossFrameLimits = 10;
    const int skipFrameBase = 20;
    const int skipFrameLimits = 3;
    struct TriggerData {
        std::function<void()> func;
    };
    enum class PlayState : uint8_t {
        NONE,
        PLAY,
        STOP,
        PAUSE,
        END,
        INVISIBLE_PAUSE
    };
    LottieHandler(std::string lottieId, std::shared_ptr<LottieConfig> &&lottieConfig);
    ~LottieHandler();
    std::shared_ptr<Animator> animator_{nullptr};
    std::unique_ptr<LottieListener> lottieListener_{nullptr};
    std::shared_ptr<DataManager> dataManager_{nullptr};
    napi_threadsafe_function safeFunc_{nullptr};

    /**
     * @brief 触发监听器事件
     * @param type 事件类型
     */
    void TriggerListenerEvent(LottieEventType type);

    /**
     * @brief 加载动画数据
     * @return 是否加载成功
     */
    bool LoadAnimationData();

    /**
     * @brief 初始化动画参数
     * @return 是否初始化成功
     */
    bool InitializeAnimationParameters();

    /**
     * @brief 配置帧率
     */
    void ConfigureFrameRate();

    /**
     * @brief 初始化所有handle相关的数据，这里涉及JSON文件解析，文件下载，rlottie的解析。
     */
    bool InitHandleData(void);

    /**
     * @brief 推进到下一帧
     */
    void AdvanceFrame();

    /**
     * @brief 推进到下一ms
     */
    void AdvanceTime(double const &elapsedTime);

    /**
     * @brief 重置动画
     * @param forceFlag 是否立即生效
     */
    void ResetSegments(bool const &forceFlag);

    /**
     * @brief 根据相对值更新绝对帧
     * @param relativeFrame 相对帧
     */
    void UpdateAbsoluteFrameToPlay(float const &relativeFrame);

    /**
     * @brief 考虑 direction_、speed_、frameRate_对每一帧的前进步长的影响，更新步长修正器
     */
    void UpdateFrameModifier();

    /**
     * @brief 更新当前segment
     * @param segment 要更新的动画片段
     * @return 播放的片段首帧
     */
    float UpdateCurrentSegment(AnimationSegment const &segment);

    /**
     * @brief 检查segments容器并更新当前segment。
     * @return 返回下一个要播放的帧编号，若无有效片段则返回 `INVALID_FRAME`
     */
    float CheckSegmentUpdate();

    /**
     * @brief 检测是否需要循环播放
     * @return 是否需要循环播放
     */
    bool CheckLoop();

    /**
     * @brief 获取当前片段实际要播放的动画帧。比如逆序播放时，第 1 帧实际上对应的是片段的最后一帧。
     * @param frame 当前帧数
     * @return nextFrame 实际应该播放的帧数
     */
    float GetNextFrameToPlay(float const &frame);

    /**
     * @brief 相对值转换为绝对帧
     * @param relativeFrame 相对帧
     * @return 绝对帧
     */
    float RelativeFrameToAbsolute(float const &relativeFrame);

    /**
     * @brief 绝对帧转换为相对值
     * @param absoluteFrame 绝对帧
     * @return 相对值
     */
    float AbsoluteFrameToRelative(float const &absoluteFrame);

    /**
     * @brief 重置lastTimestamp
     */
    void ResetLastTimeStamp();

    /**
     * @brief 播放
     */
    void Play();

    /**
     * @brief 暂停
     */
    void Pause();

    /**
     * @brief 停止
     */
    void Stop();

    /**
     * @brief 切换播放/暂停
     */
    void TogglePause();

    /**
     * @brief 控制动画画面从在某一帧或某个时刻停止
     * @param framesOrMs 帧号(值>=0)或时刻(ms)
     * @param isFrame true按帧控制, false按时间控制, 缺省默认false
     */
    void GoToAndStop(float const &framesOrMs, std::optional<bool> &isFrame);

    /**
     * @brief 控制动画画面从在某一帧或某个时刻开始播放
     * @param framesOrMs 帧号(值>=0)或时刻(ms)
     * @param isFrame true按帧控制, false按时间控制, 缺省默认false
     */
    void GoToAndPlay(float const &framesOrMs, std::optional<bool> &isFrame);

    /**
     * @brief 重新加载参数播放动画
     */
    void Reload();

    /**
     * @brief 修改动画图层颜色
     * @param path 参考rlottie.h内的介绍，是图层名称
     * @param color 修改后的颜色 支持 RGBA 和 RGB
     */
    void ChangeColor(std::string path, std::vector<float> setColor);

    /**
     * @brief 修改动画关键帧图层颜色
     * @param path 参考rlottie.h内的介绍，是图层名称
     * @param startColor 关键帧起始颜色 支持 RGBA 和 RGB
     * @param endColor 关键帧结束颜色 支持 RGBA 和 RGB
     */
    void ChangeColor(std::string path, std::vector<float> startColor, std::vector<float> endColor);

    /**
     * @brief 获得当前帧号，默认为浮点数，调用setSubframe(false) 后为整数
     */
    float const GetCurrentFrame();

    /**
     * @brief 获得当前帧号，默认为浮点数
     */
    float const GetCurrentRawFrame();

    /**
     * @brief 获得总帧数
     */
    float const GetTotalFrames();

    /**
     * @brief 获得唯一id
     */
    std::string const &GetLottieId() const;

    /**
     * @brief 获取动画宽高
     */
    LottieSize GetLottieSize();

    /**
     * @brief 获取绘制时使用的绝对帧
     */
    float const GetAbsoluteFrameToPlay();

    /**
     * @brief 获取动画源文件的总帧数
     */
    float const GetLottieFileTotalFrames();

    /**
     * @brief 获取动画源文件的帧率
     */
    float const GetLottieFileTotalFrameRate();

    /**
     * @brief 获取帧率 (frame/s)
     */
    float const GetFrameRate();

    /**
     * @brief 获取动画播放起始帧
     */
    float const GetFirstFrame();

    /**
     * @brief 获取动画播放结束帧
     */
    float const GetEndFrame();

    /**
     * @brief 获取动画播放次数
     */
    int const GetPlayCount();

    /**
     * @brief 获取当前动画是否暂停
     */
    bool const GetIsPaused();

    /**
     * @brief 获取当前是否开启子帧插值
     */
    float const GetIsSubframeEnabled();

    /**
     * @brief 获取播放速度
     */
    float const GetPlaySpeed();

    /**
     * @brief 获取播放方向
     */
    int const GetDirection();

    /**
     * @brief 获取当前动画片段完成单次播放的帧数
     */
    float const GetTimeCompleted();

    /**
     * @brief 获取当前动画片段序号
     */
    int const GetSegmentPos();

    /**
     * @brief 获取LottieView设定的参数
     */
    std::shared_ptr<LottieConfig> GetLottieConfig();

    /**
     * @brief 获取动画是否成功加载
     */
    bool const GetIsLoaded();

    /**
     * @brief 获取动画播放片段
     */
    std::vector<AnimationSegment> const GetSegments();

    /**
     * @brief 更新布局模式
     */
    std::function<void()> updateContentMode{nullptr};

    /**
     * @brief 渲染当前帧
     */
    std::function<void()> renderCurrentFrame{nullptr};

    /**
     * @brief 渲染当前帧并推进到下一个待绘制帧
     */
    std::function<void()> renderAndAdvanceFrame{nullptr};

    /**
     * @brief 设定Animator动画定时器
     * @param context ArkUI节点的Handle
     */
    //TODO:lipei--------------------[20250905]-------------------------
    //void SetAnimator(ArkUI_ContextHandle context);
    void SetAnimator();

    /**
     * @brief 设定动画播放回调
     * @param func 动画播放函数
     */
    void SetRenderCallBack(std::function<void()> func);

    /**
     * @brief 设定定时器触发回调
     * @param func 定时器触发函数
     */
    void SetAnimatorCallBack(std::function<void()> func);

    /**
     * @brief 设定当前帧号
     * @param currentFrame 当前帧号
     */
    void SetCurrentFrame(float const &currentFrame);

    /**
     * @brief 设定总帧数
     * @param totalFrames 总帧数
     */
    void SetTotalFrames(const int &totalFrames);

    /**
     * @brief 设定动画尺寸
     * @param width 宽度
     * @param height 高度
     */
    void SetLottieSize(int const &width, int const &height);

    /**
     * @brief 设定片段的更新是否立即生效
     * @param forceFlag 值为false循环下次播放的时候生效
     */
    void SetForceFlag(bool const &forceFlag);

    /**
     * @brief 设定动画播放速度
     * @param speed 速度
     */
    void SetSpeed(double const &speed);

    /**
     * @brief 设定动画播放方向
     * @param direction 播放方向
     */
    void SetDirection(int const &direction);

    /**
     * @brief 设定动画播放帧率
     * @param frameRate 播放帧率
     */
    void SetFrameRate(double const &frameRate);

    /**
     * @brief 设定布局更新函数
     * @param func 布局更新函数
     */
    void SetContentModeUpdateCallback(std::function<void()> func);

    /**
     * @brief 是否开启子帧插值
     * @param isSubframeEnabled true时开启，默认为true
     */
    void SetIsSubframeEnabled(bool const &isSubframeEnabled);

    /**
     * @brief 设定动画播放片段
     * @param segment 单个片段值
     */
    void SetSegments(AnimationSegment const &segment);

    /**
     * @brief 设定动画播放片段组
     * @param segment 多个片段值
     */
    void SetSegments(std::vector<AnimationSegment> &&segments);

    /**
     * @brief 设定动画文件播放帧率
     * @param fileFrameRate 帧率
     */
    void SetFileFrameRate(float const &fileFrameRate);

    /**
     * @brief 动画完成时执行
     */
    void Complete();

    /**
     * @brief 获得当前播放状态
     */
    PlayState GetPlayState() const;

    /**
     * @brief 设定播放状态
     * @param state 播放状态 { NONE, PLAY, STOP, PAUSE, END }
     */
    void SetPlayState(PlayState state);

public:
    std::atomic<bool> isDestroy_{false};
    // 这里不需要原子锁，丢帧本来就是模糊处理，不需要精确的值
    int64_t currentIndex_{0};
    int skip_{0};

private:
    std::string lottieId_;
    std::shared_ptr<LottieConfig> lottieConfig_{nullptr};
    std::vector<AnimationSegment> segments_;
    bool isLoaded_{false};
    bool forceFlag_;                              // 是否立刻生效, 值为false循环下次播放的时候生效
    std::vector<AnimationSegment> segmentsCache_; // 当非立刻生效时用于缓存
    double frameRate_;                            // 帧率 (frame/s)
    double frameMult_;                            // 帧率 (frame/ms)
    bool isSubframeEnabled_{true};                // 是否开启子帧插值
    int32_t width_{0};
    int32_t height_{0};
    int segmentPos_{-1}; // 当前动画片段序号
    float firstFrame_;
    float endFrame_;
    float frameModifier_; // 步长调整器
    float currentFrame_{0};
    float offsetFrame_{0};           // 帧序列的偏移量
    float totalFrames_{0};           // 当前播放片段的总帧数
    float lottieFileTotalFrames_{0}; // 动画源文件的总帧数
    float lottieFileFrameRate_{0};   // 动画源文件的帧率
    float absoluteFrameToPlay_{0};   // 绘制时使用的绝对帧
    double playSpeed_{1.0};
    Direction direction_{Direction::FORWARD};
    int playCount_{0};                                                          // 记录播放次数
    std::chrono::time_point<std::chrono::high_resolution_clock> lastTimestamp_; // 记录上一次的时间戳
    bool isPaused_{true};                                                       // 是否暂停
    float timeCompleted_{0}; // 当前动画片段完成单次播放的帧数
    std::atomic<PlayState> playState_{PlayState::NONE};
};
} // namespace LottieTurbo
#endif
