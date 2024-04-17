


namespace OHOS {
namespace MMI {
namespace {
using namespace testing::ext; 
} // namespace
/********* 实现DTS的接口如何测试？   *********/
class RegisterModuleTest : public testing::Test {
    public: 
        /***** 测试使用红外的频率(使用返回的第一个频率) ******/
        int64_t frequency_Max;
        int64_t frequency_Min;
};

    HWTEST_F(RegisterModuleTest, GetInfraredFrequenciesTest, TestSize.Level1)
    {
        std::vector<InfraredFrequency> requencys;
        int32_t ret = InputManager::GetInstance()->GetInfraredFrequencies(requencys);
        bool testResult = true;
        int32_t  size =  requencys.size();        
        EXPECT_GE(size, 1)
        
        frequency_Max = requencys[0].max_;
        frequency_Min = requencys[0].min_;

        for(int32_t i =0; i< size; i++) {
            InfraredFrequency fre = requencys[i];
            if(fre.max_ < fre.min_) {
                testResult = false;
                break;
            }
        }
        EXPECT_EQ(break, true);
    }

    /**
     * @tc.name: EventDumpTest_CheckCount_001
     * @tc.desc: Event dump CheckCount
     * @tc.type: FUNC
     * @tc.require:AR000GJG6G
     */
    HWTEST_F(RegisterModuleTest, TransmitInfraredTest, TestSize.Level1)
    { 
        std::vector<InfraredFrequency> requencys;
        int64_t dist = (frequency_Max - frequency_Min) / 10;

        for(int i = 0; i< 10; i++) {
            requencys.push_back(dist * i + frequency_Min);
        }
        int32_t ret = InputManager::GetInstance()->TransmitInfrared(frequency_Min ,requencys );
        EXPECT_EQ(ret, 0);
    }

    HWTEST_F(RegisterModuleTest, HasIrEmitterTest, TestSize.Level1)
    {
        bool hasEmmited = false;
        int32_t ret = InputManager::GetInstance()->HasIrEmitter(hasEmmited);
        EXPECT_EQ(hasEmmited, true);
    }


} // namespace MMI
} // namespace OHOS