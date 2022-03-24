package com.jd.platform.jlog.core;

import com.alibaba.fastjson.JSON;
import com.jd.platform.jlog.common.handler.JcProperties;
import com.jd.platform.jlog.common.utils.CollectionUtil;
import com.jd.platform.jlog.common.utils.StringUtil;
import io.netty.util.concurrent.DefaultThreadFactory;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.yaml.snakeyaml.Yaml;

import java.io.*;
import java.net.URL;
import java.util.*;
import java.util.concurrent.*;
import java.util.concurrent.atomic.AtomicBoolean;

import static com.jd.platform.jlog.core.ConfiguratorFactory.useFileConfig;
import static com.jd.platform.jlog.core.Constant.*;


/**
 * @author tangbohu
 * @version 1.0.0
 * @ClassName FileConfigurator.java
 * @createTime 2022年02月17日 23:22:00
 */
public class FileConfigurator implements Configurator {

    private static final Logger LOGGER = LoggerFactory.getLogger(FileConfigurator.class);

    private static JcProperties PROPERTIES = new JcProperties();

    private static volatile FileListener FILELISTENER = null;

    private static final Set<String> LISTENED_FILES = new CopyOnWriteArraySet<>();

    private final Map<String, FileWrapper> FILE_MODIFY_MAP = new ConcurrentHashMap<>();


    public FileConfigurator() throws IOException{

        String env = System.getenv(ENV);

        for (String file : CONFIG_FILES) {
            String fileName = StringUtil.isEmpty(env) ? file : file + "_" + env;
            URL url = this.getClass().getResource(file);
            if(url == null){
                continue;
            }
            try (InputStream is = url.openStream()) {
                JcProperties props = new JcProperties();
                if (fileName.contains(YML)) {
                    props.putAll(new Yaml().loadAs(is, Map.class));
                } else {
                    props.load(is);
                }
                FILE_MODIFY_MAP.put(fileName, new FileWrapper(new File(url.getFile()).lastModified(), props));
                PROPERTIES.putAll(props);
                LOGGER.debug("{}配置文件配置:{}", file, props.toString());
            }
        }
        LOGGER.debug("合并后的配置:{}",PROPERTIES.toString());
        for (String file : CONFIG_FILES) {
            file = StringUtil.isEmpty(env) ? file : file + "_" + env;
            LISTENED_FILES.add(file);
            if(FILELISTENER == null){
                synchronized (FileConfigurator.class){
                    FILELISTENER = new FileListener();
                    FILELISTENER.addListener();
                }
            }
        }
    }


    @Override
    public String getString(String key) {
        return PROPERTIES.getString(key);
    }

    @Override
    public Long getLong(String key) {
        return PROPERTIES.getLong(key);
    }

    @Override
    public List<String> getList(String key) {
        return PROPERTIES.getStrList(key);
    }

    @Override
    public <T> T getObject(String key, Class<T> clz) {
        return PROPERTIES.getBean(key, clz);
    }
    
    @Override
    public boolean putConfig(String key, String content) { return false; }


    @Override
    public boolean putConfig(String key, String content, long timeoutMills) {
        return false;
    }


    @Override
    public String getType() {
        return "file";
    }

    

    class FileListener implements ConfigChangeListener {

        private final ExecutorService executor = new ThreadPoolExecutor(1, 1, 0L,
                TimeUnit.MILLISECONDS, new LinkedBlockingQueue<>(),
                new DefaultThreadFactory("fileListener", 1));


        FileListener() {}

        synchronized void addListener() {
            FILELISTENER.onProcessEvent(new ConfigChangeEvent());
        }


        @Override
        public void onChangeEvent(ConfigChangeEvent event) {
            for(; !getExecutorService().isShutdown() && !getExecutorService().isShutdown(); checkAndConfigure()) {
                try {
                    if(!useFileConfig.get()){
                        LOGGER.info("装配了配置中心, 文件配置器关闭");
                        executor.shutdown();
                        if(!executor.awaitTermination(AWAIT_TIME, TimeUnit.MILLISECONDS)){
                            // 超时的时候向线程池中所有的线程发出中断(interrupted)。
                            executor.shutdownNow();
                            return;
                        }
                    }
                    Thread.sleep(LISTENER_CONFIG_INTERVAL);
                } catch (InterruptedException ignored) {
                }
            }
        }

        @Override
        public ExecutorService getExecutorService() {
            return executor;
        }

    }




    /**
     * 检测文件最后修改时间 和重载文件
     */
    private void checkAndConfigure(){

        AtomicBoolean change = new AtomicBoolean(false);
        Map<String, FileWrapper> newModifyMap = checkAndReload();

        FILE_MODIFY_MAP.forEach((k, v)->{
            FileWrapper newFile = newModifyMap.get(k);
            if(newFile != null && newFile.change){
                Set<String> diffKeys = CollectionUtil.diffKeys(newFile.props, v.props);
                if(!diffKeys.isEmpty()){
                    change.set(true);
                    for (String diffKey : diffKeys) {
                            LOGGER.warn("文件 {} 配置变更 key={}变更事件:{}", k, diffKey, new ConfigChangeEvent(diffKey, String.valueOf(v.props.get(diffKey)), String.valueOf(newFile.props.get(diffKey))));
                    }
                    v.props = newFile.props;
                    v.lastModify= newFile.lastModify;
                }
            }
        });
        if(change.get()){
            LOGGER.debug("变更之前的总配置：{}", JSON.toJSONString(PROPERTIES));
            PROPERTIES.clear();
            FILE_MODIFY_MAP.forEach((k,v)-> PROPERTIES.putAll(v.props));
            LOGGER.info("变更之后的总配置：{}", JSON.toJSONString(PROPERTIES));
            ClientHandlerBuilder.refresh();
        }
    }


    /**
     * 检测文件最后修改时间 和重载文件
     */
    private Map<String, FileWrapper> checkAndReload() {

        Map<String, FileWrapper> fileWrapperMap = new ConcurrentHashMap<>(3);

        for (String fileName : LISTENED_FILES) {
            URL url = this.getClass().getResource(fileName);

            if(url == null){
                continue;
            }
            File file = new File(url.getFile());
            long curLastMod = file.lastModified();
            FileWrapper wrapper = FILE_MODIFY_MAP.get(fileName);
            Long cacheLastMod = wrapper.lastModify;
            if(curLastMod <= cacheLastMod){
                continue;
            }
            JcProperties props = new JcProperties();
            try (InputStream is = url.openStream()) {
                if (fileName.contains(YML)) {
                    props.putAll(new Yaml().loadAs(is, Map.class));
                } else {
                    props.load(is);
                }
                fileWrapperMap.put(fileName, new FileWrapper(curLastMod, true, props));
                LOGGER.info("定时任务监测到文件更新：{}最新配置:{}", fileName, JSON.toJSONString(props));
            } catch (Exception e) {
                LOGGER.error("定时任务load失败：{}配置文件配置重载失败", fileName, e);
            }
        }
        return fileWrapperMap;
    }


    static class FileWrapper{

        private long lastModify;

        private boolean change;

        private JcProperties props;

        FileWrapper(long lastModify, JcProperties props) {
            this.lastModify = lastModify;
            this.change = false;
            this.props = props;
        }
        FileWrapper(long lastModify, boolean change, JcProperties props) {
            this.lastModify = lastModify;
            this.change = change;
            this.props = props;
        }
    }
}
