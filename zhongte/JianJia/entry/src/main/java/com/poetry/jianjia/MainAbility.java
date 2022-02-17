package com.poetry.jianjia;

import com.poetry.jianjia.slice.MainAbilitySlice;

import ohos.aafwk.ability.fraction.FractionAbility;
import ohos.aafwk.content.Intent;

public class MainAbility extends FractionAbility {
    @Override
    public void onStart(Intent intent) {
        super.onStart(intent);
        super.setMainRoute(MainAbilitySlice.class.getName());
    }
}
