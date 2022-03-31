import Vue from 'vue'
import Vuex from 'vuex'

Vue.use(Vuex)

const store=new Vuex.Store({
    state:{
        total:sessionStorage.getItem("total")?sessionStorage.getItem("total"):'',
        used:sessionStorage.getItem("used")?sessionStorage.getItem("used"):'',
        remain:sessionStorage.getItem("remain")?sessionStorage.getItem("remain"):'',
        auth:sessionStorage.getItem("auth")?sessionStorage.getItem("auth"):'',
        cardStatus:sessionStorage.getItem("cardStatus")?sessionStorage.getItem("cardStatus"):'',
        appid:sessionStorage.getItem("appId")?sessionStorage.getItem("appId"):'',
        iccid:sessionStorage.getItem("iccid")?sessionStorage.getItem("iccid"):'',
        openid:sessionStorage.getItem("openid")?sessionStorage.getItem("openid"):'',
        cardType:sessionStorage.getItem("cardType")?sessionStorage.getItem("cardType"):'',
    },
    mutations:{
        setTotal(state,val){
            state.total=val
            sessionStorage.setItem("total",val)
        },
        setUsed(state,val){
            state.used=val
            sessionStorage.setItem("used",val)
        },
        setRemain(state,val){
            state.remain=val
            sessionStorage.setItem("remain",val)
        },
        setAuth(state,val){
            state.auth=val
            sessionStorage.setItem("auth",val)
        },
        setCardStatus(state,val){
            state.cardStatus=val
            sessionStorage.setItem("cardStatus",val)
        },
        setAppid(state,val){
            state.appid=val
            sessionStorage.setItem("appId",val)
        },
        setIccid(state,val){
            state.iccid=val
            sessionStorage.setItem("iccid",val)
        },
        setOpenid(state,val){
            state.openid=val
            sessionStorage.setItem("openid",val)
        },
        setCardType(state,val){
            state.cardType=val
            sessionStorage.setItem("cardType",val)
        },

        removeIccid(state,val){
            state.iccid=""
            sessionStorage.removeItem("iccid")
        },
        removeOpenid(state,val){
            state.openid=""
            sessionStorage.removeItem("openid")
        },
        removeCardType(state,val){
            state.cardType=""
            sessionStorage.removeItem("cardType")
        },
        removeAppid(state,val){
            state.appid=""
            sessionStorage.removeItem("appId")
        },

        removeTotal(state,val){
            state.total=""
            sessionStorage.removeItem("total")
        },
        removeUsed(state,val){
            state.used=""
            sessionStorage.removeItem("used")
        },
        removeRemain(state,val){
            state.remain=""
            sessionStorage.removeItem("remain")
        },
        removeAuth(state,val){
            state.auth=""
            sessionStorage.removeItem("auth")
        },
        removeCardStatus(state,val){
            state.cardStatus=""
            sessionStorage.removeItem("cardStatus")
        },
    },

})

export default store