#include "../common/Androidutils_feas.h"
#include "../common/S3profile.h"

inline std::string getGov(){
    std::ifstream fd("/sys/devices/system/cpu/cpufreq/policy4/scaling_governor");
    std::string gov;
    fd >> gov;
    fd.close();
    return gov;
}

//stop feas boost_affinity if had installed asoulopt
inline void Ifasopt()
{
    if(Testfile("/data/adb/modules/asoul_affinity_opt/AsoulOpt"))
    {
        Lockvalue("/sys/module/mtk_fpsgo/parameters/boost_affinity", 0);
        Lockvalue("/sys/module/perfmgr/parameters/boost_affinity", 0);
    }
    else
    {
        Lockvalue("/sys/module/mtk_fpsgo/parameters/boost_affinity", 1);
        Lockvalue("/sys/module/perfmgr/parameters/boost_affinity", 1);
    }
}

int main(int argc, char* argv[])
{
    std::cout << argv[0] << ": Start running" << std::endl;

    //read profile
    const char *Profilelocat = argv[1];
    listProfile profile(Profilelocat);
    if(!profile.Readsucces)
    {
        //read error
        std::cout << "Profile error" << std::endl;
        return 1;
    }
    std::cout << "Read profile done." << std::endl;
    profile.List();
    //read done
    
    //update-profile daemon
    profile.startProfilemonitor(5);
    
    //creat device
    roidDeviceFeas device("feas");

    //test feas support
    if (!device.ifFeas_support())
    {
        std::cout << "Not support feas, or this sortware is out of date." << std::endl;
        //not support
        return 1;
    }
    std::cout << "Support." << std::endl;
    std::cout << "Device: " << device.getType() << std::endl;

    //start Topappmonitor
    device.startTopappmonitor(3);
    while(true)
    {
        if(profile.Inlist(device.getToppkg()))//is a game in config
        {
            Ifasopt();
            
            //open feas
            device.Feason(profile.fps);
            
            //swich cpu 0-7 to performance
            while(getGov() != std::string("performance"))
            {
                Lockvalue("/sys/devices/system/cpu/cpufreq/policy4/scaling_governor", "performance");
                Lockvalue("/sys/devices/system/cpu/cpufreq/policy7/scaling_governor", "performance");
            }
            
            /*From mi joyose config
             *now only genshin*/
            if(device.getType() == "qcom")
            {
                if(isOP(device)) //if game is OP
                /*from k50u joyose config*/
                {
                    Lockvalue("/sys/module/perfmgr/parameters/load_scaling_y", 1);
                    Lockvalue("/sys/module/perfmgr/parameters/load_scaling_a", 280);
                    Lockvalue("/sys/module/perfmgr/parameters/load_scaling_b", int(-40));
                    Lockvalue("/sys/module/perfmgr/parameters/normal_frame_keep_count", 10);
                    Lockvalue("/sys/module/perfmgr/parameters/predict_freq_level", 0);
                }
                else //others
                {
                    Lockvalue("/sys/module/perfmgr/parameters/load_scaling_y", 1);
                    Lockvalue("/sys/module/perfmgr/parameters/load_scaling_a", 450);
                    Lockvalue("/sys/module/perfmgr/parameters/load_scaling_b", int(-70));
                    Lockvalue("/sys/module/perfmgr/parameters/normal_frame_keep_count", 8);
                    Lockvalue("/sys/module/perfmgr/parameters/predict_freq_level", 0);
                }
            }
            if(device.getType() == "mtk")
            {
                if(isNewFeas()) // newer feas on mtk(example k60e) feas >= 2.2
                {
                    if(isOP(device))
                    {
                        Lockvalue("/sys/module/mtk_fpsgo/parameters/scaling_a", 400);
                        Lockvalue("/sys/module/mtk_fpsgo/parameters/scaling_b", int(-50));
                        Lockvalue("/sys/module/mtk_fpsgo/parameters/load_scaling_x", 6);
                        Lockvalue("/sys/module/mtk_fpsgo/parameters/load_scaling_y", int(-1));
                        Lockvalue("/sys/module/mtk_fpsgo/parameters/normal_frame_keep_count", 10);
                        Lockvalue("/sys/module/mtk_fpsgo/parameters/continus_no_jank_count", 15);
                        Lockvalue("/sys/module/mtk_fpsgo/parameters/scaling_a_thres", 800);
                        Lockvalue("/sys/module/mtk_fpsgo/parameters/predict_freq_level", 1);
                    }
                    else //not OP
                    {
                        Lockvalue("/sys/module/mtk_fpsgo/parameters/scaling_a", 500);
                        Lockvalue("/sys/module/mtk_fpsgo/parameters/predict_freq_level", 1);
                        Lockvalue("/sys/module/mtk_fpsgo/parameters/load_scaling_x", 5);
                        Lockvalue("/sys/module/mtk_fpsgo/parameters/normal_frame_keep_count", 8);
                    }
                }
                else if(!isNewFeas()) // older feas on mtk(example k50) feas <=2.1
                {
                    if(isOP(device))
                    {
                        Lockvalue("/sys/module/mtk_fpsgo/parameters/scaling_a", 400);
                        Lockvalue("/sys/module/mtk_fpsgo/parameters/scaling_b", int(-50));
                        Lockvalue("/sys/module/mtk_fpsgo/parameters/normal_frame_keep_count", 10);
                        Lockvalue("/sys/module/mtk_fpsgo/parameters/scaling_a_thres", 800);
                        Lockvalue("/sys/module/mtk_fpsgo/parameters/predict_freq_level", 1);
                    }
                    else //not OP
                    {
                        Lockvalue("/sys/module/mtk_fpsgo/parameters/scaling_a", 500);
                        Lockvalue("/sys/module/mtk_fpsgo/parameters/predict_freq_level", 1);
                        Lockvalue("/sys/module/mtk_fpsgo/parameters/load_scaling_x", 5);
                        Lockvalue("/sys/module/mtk_fpsgo/parameters/normal_frame_keep_count", 8);
                    }
                }
            }
        }
        else
        {
            //close feas
            device.Feasoff();
            
            //swich performance to schedutil/walt
            while(getGov() != "walt" && getGov() != "schedutil")
            {
                if(device.getType() == "qcom")
                {
                    Lockvalue("/sys/devices/system/cpu/cpufreq/policy4/scaling_governor", "walt");
                    Lockvalue("/sys/devices/system/cpu/cpufreq/policy7/scaling_governor", "walt");
                    if(getGov() != "walt") //fall back
                    {
                        Lockvalue("/sys/devices/system/cpu/cpufreq/policy4/scaling_governor", "schedutil");
                        Lockvalue("/sys/devices/system/cpu/cpufreq/policy7/scaling_governor", "schedutil");
                    }
                }
                if(device.getType() == "mtk")
                {
                    Lockvalue("/sys/devices/system/cpu/cpufreq/policy4/scaling_governor", "schedutil");
                    Lockvalue("/sys/devices/system/cpu/cpufreq/policy7/scaling_governor", "schedutil");
                }
            }
        }
        //dumpsys update in 3s
        sleep(3);
    }
}