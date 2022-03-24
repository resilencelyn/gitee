package com.jd.platform.jlog.etcd;

import com.google.protobuf.ByteString;
import com.ibm.etcd.api.Event;
import com.ibm.etcd.api.KeyValue;
import com.ibm.etcd.api.RangeResponse;
import com.ibm.etcd.client.EtcdClient;
import com.ibm.etcd.client.kv.KvClient;
import com.ibm.etcd.client.kv.WatchUpdate;
import com.jd.platform.jlog.common.handler.JcProperties;
import com.jd.platform.jlog.common.utils.CollectionUtil;
import com.jd.platform.jlog.common.utils.StringUtil;
import com.jd.platform.jlog.core.ClientHandlerBuilder;
import com.jd.platform.jlog.core.ConfigChangeEvent;
import com.jd.platform.jlog.core.ConfigChangeListener;
import com.jd.platform.jlog.core.ConfigChangeType;
import io.netty.util.concurrent.DefaultThreadFactory;

import java.io.IOException;
import java.io.StringReader;
import java.util.List;
import java.util.Set;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.LinkedBlockingQueue;
import java.util.concurrent.ThreadPoolExecutor;
import java.util.concurrent.TimeUnit;

import static com.jd.platform.jlog.etcd.EtcdConfigurator.PROPERTIES;
import static com.jd.platform.jlog.etcd.EtcdConfigurator.PROPERTIES_PATH;
import static com.jd.platform.jlog.etcd.EtcdConfigurator.ROOT;

/**
 * @author tangbohu
 * @version 1.0.0
 * @ClassName EtcdListener.java
 * @Description TODO
 * @createTime 2022年02月21日 23:34:00
 */
public class EtcdListener implements ConfigChangeListener {
    private KvClient.WatchIterator iterator;

    public EtcdListener() {

        iterator = EtcdConfigurator.client.getKvClient().watch(ByteString.copyFromUtf8(ROOT)).asPrefix().start();
        getExecutorService().submit(() -> {
            while (iterator.hasNext()){
                try {
                    WatchUpdate update = iterator.next();
                    Event eve = update.getEvents().get(0);
                    KeyValue kv = eve.getKv();
                    Event.EventType eveType = eve.getType();
                    ConfigChangeType changeType = eveType.equals(Event.EventType.DELETE) ? ConfigChangeType.MODIFY : ConfigChangeType.DELETE;
                    ConfigChangeEvent event = new ConfigChangeEvent();
                    event.setKey(kv.getKey().toStringUtf8()).setNewValue(kv.getValue().toStringUtf8()).setChangeType(changeType);
                    onChangeEvent(event);
                }catch (RuntimeException e){
                    e.printStackTrace();
                }

            }
        });
    }


    @Override
    public void onShutDown() {
        this.iterator.close();
        getExecutorService().shutdownNow();
    }


    @Override
    public void onChangeEvent(ConfigChangeEvent event) {

        RangeResponse rangeResponse = EtcdConfigurator.client.getKvClient().get(ByteString.copyFromUtf8(PROPERTIES_PATH)).sync();
        List<KeyValue> keyValues = rangeResponse.getKvsList();
        if (CollectionUtil.isEmpty(keyValues)) {
            return;
        }
        String val = keyValues.get(0).getValue().toStringUtf8();
        JcProperties props = new JcProperties();
        if(StringUtil.isNotBlank(val)){
            try {
                props.load(new StringReader(val));
            } catch (IOException e) {
                e.printStackTrace();
            }

            Set<String> diffKeys = CollectionUtil.diffKeys(props, PROPERTIES);
            if(!diffKeys.isEmpty()){
                PROPERTIES = props;
                for (String diffKey : diffKeys) {
                    LOGGER.warn("NACOS {} 配置变更 key={} 变更事件:{}", event.getKey(), diffKey,
                            String.valueOf(props.get(diffKey)),
                            String.valueOf(PROPERTIES.get(diffKey)));
                }
                ClientHandlerBuilder.refresh();
            }
        }
    }

}
