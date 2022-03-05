package middle

import (
	_ "gitee.com/wheat-os/wheatCache/conf"
	"gitee.com/wheat-os/wheatCache/pkg/event"
	"gitee.com/wheat-os/wheatCache/plugins"
	"gitee.com/wheat-os/wheatCache/plugins/config"
	"github.com/spf13/viper"
)

type MiddleWare struct {
	eventDriver   event.DriverInterface
	eventConsumer event.ConsumerInterface
	eventProduce  event.ProduceInterface
	plugins       map[string][]plugins.PluginInterface
	consumerCount int
	driverCount   int
}

func NewMiddleWare() *MiddleWare {
	oneMiddle.Do(func() {
		consumerCount, driverCount := loadConfigAndDefault()

		driver := event.NewDriver(driverCount)
		middleWareDriver = &MiddleWare{
			eventDriver:   driver,
			eventConsumer: event.NewConsumer(driver),
			eventProduce:  event.NewProduce(driver),
			driverCount:   driverCount,
			consumerCount: consumerCount,
		}
		middleWareDriver.loadPlugins()

		// 多消费 middle
		middleWareDriver.startWork()
	})
	return middleWareDriver
}

func (m *MiddleWare) GetEventDriver() event.DriverInterface {
	return m.eventDriver
}

func (m *MiddleWare) loadPlugins() {
	plug := viper.GetStringMapStringSlice("plugins-control")

	pluginsMap := config.GetMiddlewareMap()

	pluginsContext := make(map[string][]plugins.PluginInterface)

	for msg, pluNames := range plug {
		pulgSingle := make([]plugins.PluginInterface, 0)
		for _, name := range pluNames {
			pulgSingle = append(pulgSingle, pluginsMap[name])
		}

		pluginsContext[msg] = pulgSingle
	}

	m.plugins = pluginsContext
}

func loadConfigAndDefault() (int, int) {
	// 加载 consumerCount
	consumerCount := viper.GetInt("middle-driver.middleConsumerCount")
	if consumerCount == 0 {
		consumerCount = defaultConsumerCount
	}
	driverCount := viper.GetInt("middle-driver.driverCount")
	if driverCount == 0 {
		driverCount = defaultDriverCount
	}
	return consumerCount, driverCount
}
