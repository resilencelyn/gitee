package cn.chongho.inf.flink.model;

import com.google.gson.Gson;
import lombok.Data;

@Data
public class WebResult {

	private static final int SUCCESS_CODE = 1;
	private static final String SUCCESS = "请求成功";


	private static final int ERROR = 2;

	private static final int NO_AUTHORITY = 303;
	private static final String NOAUTHORITYMSG = "没有权限";

	private static final int NO_LOGIN = 304;
	private static final String NOLOGINMSG = "没有登录";

	private static final int NEED_PARAMS_CODE = 1002;
	private static final String NEEDPARAMS = "请输入参数：";

	private static final int UNKNOWN_CODE = 1001;
	private static final String UNKNOWN = "未知异常";

	private static final int NODATA_CODE = 2000;
	private static final String NODATA = "没有数据了";

	private static final int ERROR_SIGN = 3000;
	private static final String ERRORSIGNSTR = "请不要尝试破解接口";
	private int code;
	private String msg;
	private Object obj;

	public WebResult() {
		super();
	}
	public WebResult(int code, String msg) {
		super();
		this.code = code;
		this.msg = msg;
	}

	public WebResult(int code, String msg, Object obj) {
		super();
		this.code = code;
		this.msg = msg;
		this.obj = obj;
	}


	@Override
	public String toString() {
		return new Gson().toJson(this);
	}
	public Object getObj() {
		return obj;
	}
	public void setObj(Object obj) {
		this.obj = obj;
	}
	public static WebResult success(){
		return new WebResult(SUCCESS_CODE, SUCCESS);
	}
	public static WebResult successMessage(String msg){
		return new WebResult(SUCCESS_CODE, msg);
	}
	public static WebResult success(Object object){
		return new WebResult(SUCCESS_CODE, SUCCESS,object);
	}

	/**
	 * 需要参数
	 * @param name
	 * @return
     */
	public static WebResult needParams(String name){
		return new WebResult(NEED_PARAMS_CODE, NEEDPARAMS+name);
	}

	/**
	 * 未知异常
	 * @return
     */
	public static WebResult unKnown(){
		return new WebResult(UNKNOWN_CODE, UNKNOWN);
	}

	/**
	 * 没有数据
	 * @return
     */
	public static WebResult nodata(){
		return new WebResult(NODATA_CODE, NODATA);
	}

	/**
	 * 加密错误
	 * @return
     */
	public static WebResult errorsign(){
		return new WebResult(ERROR_SIGN, ERRORSIGNSTR);
	}

	/**
	 * 没有权限
	 * @return
     */
	public static WebResult noAuthority(){
		return new WebResult(NO_AUTHORITY,NOAUTHORITYMSG);
	}

	/**
	 * 没有登录
	 * @return
	 */
	public static WebResult noLogin(){
		return new WebResult(NO_LOGIN,NOLOGINMSG);
	}

	public static WebResult error(String msg){
		return new WebResult(ERROR,msg);
	}
}
