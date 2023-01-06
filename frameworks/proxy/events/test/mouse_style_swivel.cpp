#include <iostream>
#include <chrono>
#include <thread>

#include <display_type.h>

#include "input_manager.h"
#include "mmi_log.h"
// #include "event_util_test.h"
#include "window_utils_test.h"
#include "image/bitmap.h"
#include "image_source.h"
#include "image_type.h"
#include "image_utils.h"
#include "pixel_map.h"
#include "draw/canvas.h"
#include "nocopyable.h"
#include "window.h"

#include <iostream>
#include <surface.h>
#include <type_traits>

#include "include/core/SkBitmap.h"
#include "include/core/SkColor.h"
#include "include/core/SkImage.h"
#include "include/core/SkPaint.h"
#include "include/core/SkRect.h"
#include "include/core/SkTileMode.h"
#include "modifier/rs_extended_modifier.h"

#include "include/core/SkCanvas.h"
#include "include/core/SkImageInfo.h"
#include "render/rs_border.h"
#include "transaction/rs_transaction.h"
#include "ui/rs_root_node.h"
#include "ui/rs_display_node.h"
#include "ui/rs_surface_node.h"
#include "ui/rs_surface_extractor.h"
#include "ui/rs_ui_director.h"
#include "animation/rs_ui_animation_manager.h"
#include "modifier/rs_property_modifier.h"

using namespace OHOS;
using namespace OHOS::Rosen;
using namespace std;

std::shared_ptr<RSNode> rootNode;
std::vector<std::shared_ptr<RSCanvasNode>> nodes;

void Init(std::shared_ptr<RSUIDirector> rsUiDirector, int width, int height)
{
    std::cout << "rs app demo Init Rosen Backend!" << std::endl;

    rootNode = RSRootNode::Create();
    rootNode->SetBounds(10, 10, width, height);
    rootNode->SetFrame(10, 10, width, height);
    rootNode->SetBackgroundColor(SK_ColorRED);
    rsUiDirector->SetRoot(rootNode->GetId());

    nodes.emplace_back(RSCanvasNode::Create());
    nodes[0]->SetBounds(10, 10, 50, 50);
    nodes[0]->SetFrame(10, 10, 50, 50);
    nodes[0]->SetBackgroundColor(SK_ColorBLACK);
    rootNode->AddChild(nodes[0], -1);
}

shared_ptr<Media::PixelMap> DecodeImageToPixelMap(const std::string &imagePath)
{
    OHOS::Media::SourceOptions opts;
    opts.formatHint = "image/png";
    uint32_t ret = 0;
    auto imageSource = OHOS::Media::ImageSource::CreateImageSource(imagePath, opts, ret);
    std::set<std::string> formats;
    imageSource->GetSupportedFormats(formats);
    OHOS::Media::DecodeOptions decodeOpts;
    decodeOpts.desiredSize = {
        .width = 40,
        .height = 40
    };
    decodeOpts.allocatorType = OHOS::Media::AllocatorType::SHARE_MEM_ALLOC;

    shared_ptr<Media::PixelMap> pixelMap = imageSource->CreatePixelMap(decodeOpts, ret);
    return pixelMap;
}

int main()
{
    // Init demo env
    int cnt = 0;
    std::cout << "rs app demo start!" << std::endl;
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowType(WindowType::WINDOW_TYPE_FLOAT);
    option->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
    option->SetWindowRect({ 10, 10, 50, 50 });
    auto window = Window::Create("mouse_move", option);

    window->Show();
    auto rect = window->GetRect();
    while (rect.width_ == 0 && rect.height_ == 0) {
        std::cout << "rs app demo create window failed: " << rect.width_ << " " << rect.height_ << std::endl;
        window->Hide();
        window->Destroy();
        window = Window::Create("app_demo", option);
        window->Show();
        rect = window->GetRect();
    }
    std::cout << "rs app demo create window " << rect.width_ << " " << rect.height_ << std::endl;
    auto surfaceNode = window->GetSurfaceNode();

    // Build rosen renderThread & create nodes
    auto rsUiDirector = RSUIDirector::Create();
    rsUiDirector->Init();
    RSTransaction::FlushImplicitTransaction();
    sleep(1);
    rsUiDirector->SetRSSurfaceNode(surfaceNode);
    Init(rsUiDirector, rect.width_, rect.height_);
    std::cout << "rs app demo stage " << cnt++ << std::endl;

    // change property in nodes [setter using modifier]
    rootNode->SetBackgroundColor(SK_ColorTRANSPARENT);
    std::cout << "rs app demo stage " << cnt++ << std::endl;
    nodes[0]->SetBounds({ 0.0f, 0.0f, 50.f, 50.f });
    rsUiDirector->SendMessages();
    sleep(2);

    auto unique_image = DecodeImageToPixelMap("/system/etc/multimodalinput/mouse_icon/LoadingCursor.png");
    nodes[0]->BeginRecording(10, 10);
    nodes[0]->FinishRecording();
    rsUiDirector->SendMessages();
    sleep(2);

    nodes[0]->SetBgImageWidth(41);
    nodes[0]->SetBgImageHeight(40);
    nodes[0]->SetBgImagePositionX(0);
    nodes[0]->SetBgImagePositionY(0);

    auto rosenImage = make_shared<Rosen::RSImage>();
    rosenImage->SetPixelMap(unique_image);
    rosenImage->SetImageRepeat(0);
    cout << "SetBgImage" << endl;
    nodes[0]->SetBgImage(rosenImage);

    rsUiDirector->SendMessages();

    // directly add modifier
    //nodes[0]->SetPivot(0.f, 0.f);
    auto value = Vector4f(0.f);
    auto property = std::make_shared<RSAnimatableProperty<Vector4f>>(value);
    auto modifier = std::make_shared<RSBoundsModifier>(property);
    nodes[0]->AddModifier(modifier);
    rsUiDirector->SendMessages();
    sleep(1);

    std::cout << "rs app demo stage " << cnt++ << std::endl;

    // custom drawing modifier with anim
    std::cout << "rs app demo stage " << cnt++ << std::endl;
    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(1000);
    protocol.SetRepeatCount(-1);
    RSNode::OpenImplicitAnimation(protocol, RSAnimationTimingCurve::LINEAR, []() {
        std::cout<<"animation finished"<<endl;
    });

    property->Set(360.f);
    RSNode::CloseImplicitAnimation();
    rsUiDirector->SendMessages();

    int64_t startNum = 80825861106;
    bool hasRunningAnimation = true;
    while (hasRunningAnimation) {
        hasRunningAnimation = rsUiDirector->RunningCustomAnimation(startNum);
        rsUiDirector->SendMessages();
        startNum += 100000000;
        usleep(100000);
    }

    // dump property via modifiers
    std::cout << "rs app demo stage " << cnt++ << std::endl;
    std::cout << "print: --------- " << nodes[0]->GetStagingProperties().Dump() << std::endl;
    sleep(10);

    window->MoveTo(100, 200);
    sleep(10);
    window->MoveTo(200, 300);
    sleep(10);
    window->MoveTo(300, 400);
    sleep(10);
    window->MoveTo(400, 500);
    sleep(10);
    window->MoveTo(500, 600);
    sleep(10);
    window->MoveTo(600, 700);
    sleep(10);
    std::cout << "rs app demo end!" << std::endl;
    window->Hide();
    window->Destroy();
    return 0;
}