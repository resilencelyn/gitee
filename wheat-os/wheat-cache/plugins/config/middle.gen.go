// Code generated by gen-middleware. DO NOT EDIT.
// make gen-middleware generated

package config

import (
	"gitee.com/wheat-os/wheatCache/plugins"

	mockPlugin "gitee.com/wheat-os/wheatCache/plugins/mock-plugin"
)

func GetMiddlewareMap() map[string]plugins.PluginInterface {

	mockPlugin := mockPlugin.NewPlugin()
	mockPlugin.Init()

	return map[string]plugins.PluginInterface{
		mockPlugin.Name(): mockPlugin,
	}
}