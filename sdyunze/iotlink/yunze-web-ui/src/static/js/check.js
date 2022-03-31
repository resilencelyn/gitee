
export default {
    checkIsNull:function(obj){
        if (obj === "undefined"||obj===undefined) {
            return true
        }else if(obj ==="" ||obj ===null ||
            obj ==={}||obj==="{}"){
            return true
        }else{
            return false
        }
    },
    checkPhone:function (obj) {
        let reg=/^[1][3,4,5,7,8,9][0-9]{9}$/
        if(reg.test(obj)){
            return true
        }else{
            return false
        }
    },
    checkNumber:function (obj) {
        let reg=/^[0-9]+(\.[0-9]+)?$/g
        if(!reg.test(obj)){
            return false
        }else{
            return true
        }
    },


    checkText:function (obj) {
        let reg = new RegExp("[\\u4E00-\\u9FFF]+","g");
        if (reg.test(obj)) {
            return true
        }else {
            return false
        }
    },
    checkIdCardNo:function(obj){
        let reg=/^[1-9][0-9]{5}([1][9][0-9]{2}|[2][0][0|1][0-9])([0][1-9]|[1][0|1|2])([0][1-9]|[1|2][0-9]|[3][0|1])[0-9]{3}([0-9]|[X])$/
        if(!reg.test(obj)){
            return false
        }else{
            return true
        }
    },
    checkNumAndDou:function(obj){
        let reg= /[0-9][\\,]/g
        if(!reg.test(obj)){
            return false
        }else{
            return true
        }
    },

}