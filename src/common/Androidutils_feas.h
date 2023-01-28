#pragma once
#include "Androidutils.h"

bool isOP(roidDeviceFeas &device)
{
    if(device.getToppkg() == std::string("com.miHoYo.GenshinImpact") || device.getToppkg() == std::string("com.miHoYo.Yuanshen") || device.getToppkg() == std::string("com.miHoYo.ys.bilibili") || device.getToppkg() == std::string("com.miHoYo.ys.mi")) //if game is OP
    {
        std::cout << "Genshin" << std::endl;
        return true;
    }
    return false;
}
bool isNewFeas()
{
    if(Testfile("/sys/module/mtk_fpsgo/parameters/target_fps_61"))
    {
        return true;
    }
    return false
}

// Feas Android device
class roidDeviceFeas : public roidDevice
{

private:
    bool Feas_status;
    bool Feas_support;
    std::string type;
private:
    bool getmtkFeassupport()
    {
        if (!Testfile("/sys/kernel/fpsgo/common/fpsgo_enable"))
            return false;
        if (!Testfile("/sys/module/mtk_fpsgo/parameters/perfmgr_enable"))
            return false;
        if(!Testfile("/sys/module/mtk_fpsgo/parameters/fixed_target_fps"))
            return false;
        return true;
    }
    bool getqcomFeassupport()
    {
        if (!Testfile("/sys/module/perfmgr/parameters/perfmgr_enable"))
            return false;
        if(!Testfile("/sys/module/perfmgr/parameters/target_fps_61"))
            return false;
        return true;
    }
    bool getFeassupport()
    {
        if(getmtkFeassupport())
        {
            type = "mtk";
            return true;
        }
        if(getqcomFeassupport())
        {
            type = "qcom";
            return true;
        }
        return false;
    }
public:
    roidDeviceFeas(const char *name) : roidDevice(name)
    {
        Feas_status = false;
        Feas_support = getFeassupport();
        if(getFeassupport())
            Feasoff();
    }
    inline bool Feason(int &fps)
    {
        bool tmpbool = true;
        if(type == "mtk")
        {
            if(!Lockvalue("/sys/kernel/fpsgo/common/fpsgo_enable", 1))
                tmpbool = false;
            if(!Lockvalue("/sys/module/mtk_fpsgo/parameters/perfmgr_enable", 1))
                tmpbool = false;
            if(!Lockvalue("/sys/module/mtk_fpsgo/parameters/fixed_target_fps", fps))
                tmpbool = false;
        }
        if(type == "qcom")
        {
            if(!Lockvalue("/sys/module/perfmgr/parameters/perfmgr_enable", 1))
                tmpbool = false;
            if(!Lockvalue("/sys/module/mtk_fpsgo/parameters/fixed_target_fps", fps))
            {
                if(!Lockvalue("/sys/module/mtk_fpsgo/parameters/target_fps_61", fps))
                    tmpbool = false;
            }
        }
        Feas_status = true;
        return tmpbool;
    }
    inline bool Feasoff()
    {
        bool tmpbool = true;
        if(type == "mtk")
        {
            if(!Lockvalue("/sys/kernel/fpsgo/common/fpsgo_enable", 0))
                tmpbool = false;
            if(!Lockvalue("/sys/module/mtk_fpsgo/parameters/perfmgr_enable", 0))
                tmpbool = false;
            if(!Lockvalue("/sys/module/mtk_fpsgo/parameters/fixed_target_fps"), 0)
            {
                if(!Lockvalue("/sys/module/perfmgr/parameters/target_fps_61", 0))
                {
                    tmpbool = false;
                }
            }
        }
        if(type == "qcom")
        {
            if(!Lockvalue("/sys/module/perfmgr/parameters/perfmgr_enable", 0))
                tmpbool = false;
            if(!Lockvalue("/sys/module/mtk_fpsgo/parameters/fixed_target_fps", 0))
            {
                if(!Lockvalue("/sys/module/mtk_fpsgo/parameters/target_fps_61", 0))
                    tmpbool = false;
            }
        }
        Feas_status = false;
        return tmpbool;
    }
    inline bool ifFeas_support() const
    {
        return Feas_support;
    }
    inline std::string getType() const
    {
        return type;
    }
};
