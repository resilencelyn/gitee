<template>
	<view class="nav-wrap">
		<view class="nav-title">
			<image class="logo" src="https://cdn.uviewui.com/uview/common/logo.png" mode="widthFix"></image>
			<view class="nav-info">
				<view class="nav-title__text">
					uView Vue3.0
				</view>
				<view class="nav-slogan">
					多平台快速开发UI框架
				</view>
			</view>
			<image class="logo vk-logo" src="/static/logo.png" mode="widthFix"></image>
		</view>
		<view class="nav-desc">
			<view>
				uView Vue3.0 是 uView 的 Vue3.0 版本（由 VK 维护，同时兼容 Vue2.0），其继承了 uView 1.8.3 的意志，再战江湖，风云再起。
			</view>
			<view class="mt15">
				uView 文档官网：https://v1.uviewui.com
			</view>
			<view class="mt15">
				<view>
					对部分组件的效果进行了微调（属性只增不改不删）
				</view>
				<view>
					以下只展示有区别的组件。
				</view>
			</view>
		</view>
	</view>
</template>

<script>
	export default {
		props: {
			
		},
		methods: {

		}
	}
</script>

<style lang="scss" scoped>
	.nav-wrap {
		padding: 15px;
		position: relative;
	}
	
	.lang {
		position: absolute;
		top: 15px;
		right: 15px;
	}
	
	.nav-title {
		/* #ifndef APP-NVUE */
		display: flex;
		/* #endif */
		flex-direction: row;
		align-items: center;
	}
	
	.nav-info {
		margin-left: auto;
		margin-right: auto;
	}
	
	.nav-title__text {
		/* #ifndef APP-NVUE */
		display: flex;
		/* #endif */
		color: $u-main-color;
		font-size: 25px;
		font-weight: bold;
	}
	
	.logo {
		width: 70px;
		/* #ifndef APP-NVUE */
		height: auto;
		/* #endif */
	}
	.vk-logo{
		margin-left: 30rpx;
		border-radius: 50%;
	}
	
	.nav-slogan {
		color: $u-tips-color;
		font-size: 14px;
	}
	
	.nav-desc {
		margin-top: 10px;
		font-size: 14px;
		color: $u-content-color;
	}
	
	.mt15{
		margin-top: 30rpx;
	}
</style>
