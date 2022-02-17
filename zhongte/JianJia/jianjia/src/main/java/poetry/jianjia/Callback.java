package poetry.jianjia;

/**
 * @author 裴云飞
 * @date 2021/1/23
 */

public interface Callback<T> {

    void onResponse(Call<T> call, Response<T> response);

    void onFailure(Call<T> call, Throwable t);
}
