module.exports = {
	title: 'yapi-tool使用文档',
	description: 'yapi-tool使用文档',
	themeConfig: {
		nav: [
			// {
			// 	text: '使用教程',
			// 	link: '/usage/start.html'
			// },
			{
				text: '码云',
				link: 'https://gitee.com/qdzhs/yapi-tool'
			}
		],
		sidebar: {
			'/usage/': [
				{
					title: '快速开始',
					path: '/usage/start.html',
					collapsable: false
					// children: [
					// 	'/usage/start.md'
					// ]
				},
				{
					title: 'mock-config.js配置',
					path: '/usage/config.html',
					collapsable: false
					// children: [
					// 	'/usage/config.md'
					// ]
				}
			]
		}
	}
}