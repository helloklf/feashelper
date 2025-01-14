SKIPUNZIP=0
MODDIR=${0%/*}
alias sh='/system/bin/sh'
echo "----------------------------------------------------"
# print FEATURES
cat $MODPATH/FEATURES
rm -rf $MODPATH/FEATURES

echo -e "\nPlease wait…"
echo "请等待…"
rm $MODPATH/FEATURES

# no uperf
if [ $(pidof uperf) != "" ]; then
    echo "Uperf detected, please remove."
    echo "检测到uperf，请移除"abort
fi

# remove old version
MODS_PATH="/data/adb/modules"
[ -d $MODS_PATH/Feashelper_Mtk ] && rm -rf $MODS_PATH/Feashelper_Mtk

# keep config
[ ! -f /data/feas.conf ] && cp $MODPATH/feas.conf /data/feas.conf
# mv /data/feas.conf /data/feas.conf.bak
# cp -f $MODPATH/feas.conf /data/feas.conf
# echo "Please note that this update does not preserve the configuration."

# remove model config
rm $MODPATH/feas.conf

# permission
chmod a+x $MODPATH/FEASHelper

# start FEAShelper on install
killall FEASHelper 2>&1 > /dev/null
echo "----------------------------------------------------"
$MODPATH/FEASHelper -c /data/feas.conf
sleep 1s

# test if FEASHelper started
if [[ "$(pgrep FEASHelper)" == "" ]]; then
    echo "Sorry, unsupported device."
    echo "不支持的设备"
    abort
fi
echo "FEASHelper is running……"
echo "FEASHelper已运行……"
