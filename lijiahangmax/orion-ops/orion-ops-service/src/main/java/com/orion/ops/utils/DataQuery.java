package com.orion.ops.utils;

import com.baomidou.mybatisplus.core.conditions.query.LambdaQueryWrapper;
import com.baomidou.mybatisplus.core.mapper.BaseMapper;
import com.orion.lang.wrapper.DataGrid;
import com.orion.lang.wrapper.PageRequest;
import com.orion.lang.wrapper.Pager;
import com.orion.utils.collect.Lists;
import com.orion.utils.convert.Converts;

import java.util.List;
import java.util.Optional;
import java.util.function.Function;
import java.util.stream.Collectors;
import java.util.stream.Stream;

/**
 * 数据查询器
 *
 * @author Jiahang Li
 * @version 1.0.0
 * @since 2021/5/27 17:22
 */
public class DataQuery<T> {

    private BaseMapper<T> dao;

    private PageRequest page;

    private LambdaQueryWrapper<T> wrapper;

    private DataQuery(BaseMapper<T> dao) {
        this.dao = dao;
    }

    public DataQuery(BaseMapper<T> dao, PageRequest page, LambdaQueryWrapper<T> wrapper) {
        this.dao = dao;
        this.page = page;
        this.wrapper = wrapper;
    }

    public static <T> DataQuery<T> of(BaseMapper<T> dao) {
        Valid.notNull(dao, "dao is null");
        return new DataQuery<>(dao);
    }

    public DataQuery<T> page(PageRequest page) {
        Valid.notNull(page, "page is null");
        return new DataQuery<>(dao, page, wrapper);
    }

    public DataQuery<T> wrapper(LambdaQueryWrapper<T> wrapper) {
        Valid.notNull(wrapper, "wrapper is null");
        return new DataQuery<>(dao, page, wrapper);
    }

    public Optional<T> get() {
        Valid.notNull(wrapper, "wrapper is null");
        return Optional.ofNullable(dao.selectOne(wrapper));
    }

    public <R> Optional<R> get(Class<R> c) {
        Valid.notNull(wrapper, "wrapper is null");
        return Optional.ofNullable(dao.selectOne(wrapper))
                .map(s -> Converts.to(s, c));
    }

    public Stream<T> list() {
        Valid.notNull(wrapper, "wrapper is null");
        return dao.selectList(wrapper).stream();
    }

    public <R> List<R> list(Class<R> c) {
        Valid.notNull(wrapper, "wrapper is null");
        return Converts.toList(dao.selectList(wrapper), c);
    }

    public Integer count() {
        Valid.notNull(wrapper, "wrapper is null");
        return dao.selectCount(wrapper);
    }

    public boolean present() {
        Valid.notNull(wrapper, "wrapper is null");
        return dao.selectCount(wrapper) > 0;
    }

    public DataGrid<T> dataGrid() {
        return this.dataGrid(Function.identity());
    }

    public <R> DataGrid<R> dataGrid(Class<R> c) {
        return this.dataGrid(t -> Converts.to(t, c));
    }

    public <R> DataGrid<R> dataGrid(Function<T, R> convert) {
        Valid.notNull(convert, "convert is null");
        Valid.notNull(page, "page is null");
        Valid.notNull(wrapper, "wrapper is null");
        Integer count = dao.selectCount(wrapper);
        Pager<R> pager = new Pager<>(page);
        pager.setTotal(count);
        boolean next = pager.hasMoreData();
        if (next) {
            wrapper.last(pager.getSql());
            List<R> rows = dao.selectList(wrapper).stream()
                    .map(convert)
                    .collect(Collectors.toList());
            pager.setRows(rows);
        } else {
            pager.setRows(Lists.empty());
        }
        return DataGrid.of(pager);
    }

}
