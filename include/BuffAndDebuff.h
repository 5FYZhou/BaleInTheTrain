#pragma once
#include "Constants.h"
#include "Tools/AudioManager.h"
struct BDinfo
{
    int rest_turncounts = -1; // 等于-1代表整场有效
    BuffDebuffType type = BuffDebuffType::None;
};
class Buff_Debuff_Vec
{
public:
    std::vector<BDinfo> buff_debuffs;
    Buff_Debuff_Vec() : buff_debuffs() {}
    void Add(BDinfo bd)
    {
        // 考虑相同buff叠加
        for (auto &it : buff_debuffs)
        {
            if (it.type == bd.type)
            {
                if (it.rest_turncounts != -1)
                    it.rest_turncounts += bd.rest_turncounts;
                return;
            }
        }
        buff_debuffs.push_back(bd);
        GlobalaudioMgr.PlaySound(SoundEffect::BUFFandDEBUFF);
    }
    void update() // 更新回合,删除失效buff
    {
        for (auto &it : buff_debuffs)
        {
            if (it.rest_turncounts != -1 && it.rest_turncounts != 0)
                it.rest_turncounts--;
        }
        for (int i = 0; i < buff_debuffs.size(); ++i)
        {
            if (buff_debuffs[i].rest_turncounts == 0)
            {
                buff_debuffs.erase(buff_debuffs.begin() + i);
                i--;
            }
        }
    }
    std::vector<std::pair<BuffDebuffType,int>> getBuffs(){
        std::vector<std::pair<BuffDebuffType,int>> v;
        for(auto it:buff_debuffs){
            v.push_back({it.type, it.rest_turncounts});
        }
        return v;
    }
};
