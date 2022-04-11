// Copyright (c) 2011 and onwards The OpenVanilla Project (MIT License).
// All possible vChewing-specific modifications are of:
// (c) 2021 and onwards The vChewing Project (MIT-NTL License).
/*
Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

1. The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

2. No trademark license is granted to use the trade names, trademarks, service
marks, or product names of Contributor, except as required to fulfill notice
requirements above.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

import Cocoa
import InputMethodKit

@objc(AppDelegate)
class AppDelegate: NSObject, NSApplicationDelegate, ctlNonModalAlertWindowDelegate,
	FSEventStreamHelperDelegate
{
	func helper(_ helper: FSEventStreamHelper, didReceive events: [FSEventStreamHelper.Event]) {
		// 拖 100ms 再重載，畢竟有些有特殊需求的使用者可能會想使用巨型自訂語彙檔案。
		DispatchQueue.main.asyncAfter(deadline: DispatchTime.now() + 0.1) {
			if mgrPrefs.shouldAutoReloadUserDataFiles {
				IME.initLangModels(userOnly: true)
			}
		}
	}

	// let vChewingKeyLayoutBundle = Bundle.init(path: URL(fileURLWithPath: Bundle.main.resourcePath ?? "").appendingPathComponent("vChewingKeyLayout.bundle").path)

	@IBOutlet weak var window: NSWindow?
	private var ctlPrefWindowInstance: ctlPrefWindow?
	private var ctlAboutWindowInstance: ctlAboutWindow?  // New About Window
	private var checkTask: URLSessionTask?
	private var updateNextStepURL: URL?
	private var fsStreamHelper = FSEventStreamHelper(
		path: mgrLangModel.dataFolderPath(isDefaultFolder: false),
		queue: DispatchQueue(label: "vChewing User Phrases"))
	private var currentAlertType: String = ""

	// 補上 dealloc
	deinit {
		ctlPrefWindowInstance = nil
		ctlAboutWindowInstance = nil
		checkTask = nil
		updateNextStepURL = nil
		fsStreamHelper.stop()
		fsStreamHelper.delegate = nil
	}

	func applicationDidFinishLaunching(_ notification: Notification) {
		IME.initLangModels(userOnly: false)
		fsStreamHelper.delegate = self
		_ = fsStreamHelper.start()

		mgrPrefs.setMissingDefaults()

		// 只要使用者沒有勾選檢查更新、沒有主動做出要檢查更新的操作，就不要檢查更新。
		if (UserDefaults.standard.object(forKey: VersionUpdateApi.kCheckUpdateAutomatically) != nil) == true {
			checkForUpdate()
		}
	}

	@objc func showPreferences() {
		if ctlPrefWindowInstance == nil {
			ctlPrefWindowInstance = ctlPrefWindow.init(windowNibName: "frmPrefWindow")
		}
		ctlPrefWindowInstance?.window?.center()
		ctlPrefWindowInstance?.window?.orderFrontRegardless()  // 逼著屬性視窗往最前方顯示
		ctlPrefWindowInstance?.window?.level = .statusBar
		ctlPrefWindowInstance?.window?.titlebarAppearsTransparent = true
		NSApp.setActivationPolicy(.accessory)
	}

	// New About Window
	@objc func showAbout() {
		if ctlAboutWindowInstance == nil {
			ctlAboutWindowInstance = ctlAboutWindow.init(windowNibName: "frmAboutWindow")
		}
		ctlAboutWindowInstance?.window?.center()
		ctlAboutWindowInstance?.window?.orderFrontRegardless()  // 逼著關於視窗往最前方顯示
		ctlAboutWindowInstance?.window?.level = .statusBar
		NSApp.setActivationPolicy(.accessory)
	}

	@objc(checkForUpdate)
	func checkForUpdate() {
		checkForUpdate(forced: false)
	}

	@objc(checkForUpdateForced:)
	func checkForUpdate(forced: Bool) {

		if checkTask != nil {
			// busy
			return
		}

		// time for update?
		if !forced {
			if UserDefaults.standard.bool(forKey: VersionUpdateApi.kCheckUpdateAutomatically) == false {
				return
			}
			let now = Date()
			let date = UserDefaults.standard.object(forKey: VersionUpdateApi.kNextUpdateCheckDateKey) as? Date ?? now
			if now.compare(date) == .orderedAscending {
				return
			}
		}

		let nextUpdateDate = Date(timeInterval: VersionUpdateApi.kNextCheckInterval, since: Date())
		UserDefaults.standard.set(nextUpdateDate, forKey: VersionUpdateApi.kNextUpdateCheckDateKey)

		checkTask = VersionUpdateApi.check(forced: forced) { [self] result in
			defer {
				checkTask = nil
			}
			switch result {
				case .success(let apiResult):
					switch apiResult {
						case .shouldUpdate(let report):
							updateNextStepURL = report.siteUrl
							let content = String(
								format: NSLocalizedString(
									"You're currently using vChewing %@ (%@), a new version %@ (%@) is now available. Do you want to visit vChewing's website to download the version?%@",
									comment: ""),
								report.currentShortVersion,
								report.currentVersion,
								report.remoteShortVersion,
								report.remoteVersion,
								report.versionDescription)
							IME.prtDebugIntel("vChewingDebug: \(content)")
							currentAlertType = "Update"
							ctlNonModalAlertWindow.shared.show(
								title: NSLocalizedString(
									"New Version Available", comment: ""),
								content: content,
								confirmButtonTitle: NSLocalizedString(
									"Visit Website", comment: ""),
								cancelButtonTitle: NSLocalizedString(
									"Not Now", comment: ""),
								cancelAsDefault: false,
								delegate: self)
							NSApp.setActivationPolicy(.accessory)
						case .noNeedToUpdate, .ignored:
							break
					}
				case .failure(let error):
					switch error {
						case VersionUpdateApiError.connectionError(let message):
							let title = NSLocalizedString(
								"Update Check Failed", comment: "")
							let content = String(
								format: NSLocalizedString(
									"There may be no internet connection or the server failed to respond.\n\nError message: %@",
									comment: ""), message)
							let buttonTitle = NSLocalizedString("Dismiss", comment: "")
							IME.prtDebugIntel("vChewingDebug: \(content)")
							currentAlertType = "Update"
							ctlNonModalAlertWindow.shared.show(
								title: title, content: content,
								confirmButtonTitle: buttonTitle,
								cancelButtonTitle: nil,
								cancelAsDefault: false, delegate: nil)
							NSApp.setActivationPolicy(.accessory)
						default:
							break
					}
			}
		}
	}

	func selfUninstall() {
		currentAlertType = "Uninstall"
		let content = String(
			format: NSLocalizedString(
				"This will remove vChewing Input Method from this user account, requiring your confirmation.",
				comment: ""))
		ctlNonModalAlertWindow.shared.show(
			title: NSLocalizedString("Uninstallation", comment: ""), content: content,
			confirmButtonTitle: NSLocalizedString("OK", comment: ""),
			cancelButtonTitle: NSLocalizedString("Not Now", comment: ""), cancelAsDefault: false,
			delegate: self)
		NSApp.setActivationPolicy(.accessory)
	}

	func ctlNonModalAlertWindowDidConfirm(_ controller: ctlNonModalAlertWindow) {
		switch currentAlertType {
			case "Uninstall":
				NSWorkspace.shared.openFile(
					mgrLangModel.dataFolderPath(isDefaultFolder: true), withApplication: "Finder")
				IME.uninstall(isSudo: false, selfKill: true)
			case "Update":
				if let updateNextStepURL = updateNextStepURL {
					NSWorkspace.shared.open(updateNextStepURL)
				}
				updateNextStepURL = nil
			default:
				break
		}
	}

	func ctlNonModalAlertWindowDidCancel(_ controller: ctlNonModalAlertWindow) {
		switch currentAlertType {
			case "Update":
				updateNextStepURL = nil
			default:
				break
		}
	}

	// New About Window
	@IBAction func about(_ sender: Any) {
		(NSApp.delegate as? AppDelegate)?.showAbout()
		NSApplication.shared.activate(ignoringOtherApps: true)
	}
}
