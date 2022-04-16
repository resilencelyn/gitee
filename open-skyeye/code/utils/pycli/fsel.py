#-*- coding: utf-8 -*-
import wx
import sys, os
import se_path


def path_components(path):
	#The components of an (absolute) directory path, starting with the root.
	d, b = os.path.split(path)
	if b == '':
		path = d
	comps = []
	while True:
		dname, bname = os.path.split(path)
		if dname == path:
			return [dname] + comps
		comps = [bname] + comps
		path = dname

def test_path_components():
	def expect(dir, expected_comps):
		pc = path_components(dir)
		if pc != expected_comps:
			raise Exception('path_components(%r): expected %r, got %r' % (dir, expected_comps, pc))

	if sys.platform == 'win32':
		expect('c:\\', ['c:\\'])
		expect('c:\\alfa', ['c:\\', 'alfa'])
		expect('c:\\alfa\\', ['c:\\', 'alfa'])
		expect('c:\\alfa\\beta', ['c:\\', 'alfa', 'beta'])
		expect('c:\\alfa\\beta\\', ['c:\\', 'alfa', 'beta'])
	else:
		expect('/', ['/'])
		expect('/alfa', ['/', 'alfa'])
		expect('/alfa/', ['/', 'alfa'])
		expect('/alfa/beta', ['/', 'alfa', 'beta'])
		expect('/alfa/beta/', ['/', 'alfa', 'beta'])


test_path_components()
mswindows = sys.platform == 'win32'

#该函数现在无法调用到，存在错误
'''
def drive_letters():
	drives = win32file.GetLogicalDrives()
	return [ chr(ord('A') + i) for i in xrange(26) if drives & 1 << i ]
'''

class Item(object):

	def __init__(self, dir, name):
		self.dir = dir
		self.name = name
		full = os.path.join(dir, name)
		self.is_dir = os.path.isdir(full)
		self.is_bundle = False
		if self.is_dir: 
			if full.endswith(".ckpt"):
				self.is_bundle = True
			self.hidden = False
		else:
			self.hidden = False

	def openable_dir(self):
		return self.is_dir and not self.is_bundle

	def visible(self):
		return self.name[0] != '.' and not self.hidden

	def icon(self):
		if self.openable_dir():
			return 'folder'
		elif self.is_bundle:
			return 'checkpoint'
		else:
			return 'doc'

	def sortkey(self):
		return (not self.openable_dir(), self.name.lower())


class Filter_all(object):
	label = 'All files'
	def accept(self, item):
		return True


class Filter_checkpoint(object):
	label = 'SkyEye checkpoints'
	def accept(self, item):
		return item.is_bundle


class Filter_suffix(object):
	def __init__(self, suffix, label = None):
		self.suffix = suffix
		self.label = label

	def accept(self, item):
		return item.name.endswith(self.suffix)


class Filter_union(object):
	def __init__(self, filters, label = None):
		self.filters = filters
		self.label = label

	def accept(self, item):
		return any((f.accept(item) for f in self.filters))


class Filter_intersection(object):
	def __init__(self, filters, label = None):
		self.filters = filters
		self.label = label

	def accept(self, item):
		return all((f.accept(item) for f in self.filters))


def allowed_filename_char(c):
	if mswindows:
		return c not in ':/\\<>"|?*'
	else:
		return c != '/'


class File_selector(wx.Dialog):

	def __init__(self, parent, title, starting_dir, filters = [Filter_all()], save_name = None, allow_overwrite = True, image_path = ['.'], select_suffix = False):
		wx.Dialog.__init__(self, parent, -1, title)
		self.dir = starting_dir
		self.filters = filters
		self.saving = save_name != None
		self.allow_overwrite = allow_overwrite
		self.topsizer = wx.BoxSizer(wx.VERTICAL)
		self.choice = wx.Choice(self, wx.ID_ANY)
		self.hsizer_left = wx.BoxSizer(wx.HORIZONTAL)
		self.hsizer_left.Add(self.choice, proportion=1, flag=wx.RIGHT | wx.EXPAND)
		self.up_button = wx.Button(self, wx.ID_ANY, '后退')
		self.Bind(wx.EVT_BUTTON, self.up_pressed, self.up_button)
		self.hsizer_left.Add(self.up_button, proportion=0, flag=wx.LEFT, border=16)
		self.topsizer.Add(self.hsizer_left, proportion=0, flag=wx.ALL | wx.EXPAND, border=16)
		self.Bind(wx.EVT_CHOICE, self.dir_selected, self.choice)
		st = wx.LC_REPORT | wx.LC_NO_HEADER | wx.LC_SINGLE_SEL | wx.VSCROLL | wx.BORDER_SIMPLE
		self.lbox = wx.ListCtrl(self, -1, style=st)
		self.lbox.InsertColumn(0, '')
		self.lbox.SetColumnWidth(0, 24)
		self.lbox.InsertColumn(1, '')
		self.lbox.SetColumnWidth(1, 340)

		def lookup_image(f):
			for d in image_path:
				path = os.path.join(d, f)
				if os.path.exists(path):
					return path

		self.icons = {}
		self.il = wx.ImageList(16, 16)
		for ic in ['folder', 'doc', 'checkpoint']:
			img = wx.Bitmap(se_path.GetPicPath('fsel-' + ic + '.png'), wx.BITMAP_TYPE_PNG)
			self.icons[ic] = self.il.Add(img)

		self.lbox.SetImageList(self.il, wx.IMAGE_LIST_SMALL)
		sbar_width = wx.SystemSettings.GetMetric(wx.SYS_VSCROLL_X, self.lbox)
		self.lbox.SetMinSize((364 + sbar_width + 2, 320))
		self.topsizer.Add(self.lbox, proportion=1, flag=wx.LEFT | wx.RIGHT, border=16)
		self.Bind(wx.EVT_LIST_ITEM_SELECTED, self.item_selected, self.lbox)
		self.Bind(wx.EVT_LIST_ITEM_DESELECTED, self.item_deselected, self.lbox)
		self.Bind(wx.EVT_LIST_ITEM_ACTIVATED, self.item_activated, self.lbox)
		self.cancel_button = wx.Button(self, wx.ID_CANCEL, label = "取消")
		self.confirm_button = wx.Button(self, wx.ID_OPEN, label = "确定")
		if self.saving:
			self.entry_label = wx.StaticText(self, wx.ID_ANY, label = '保存:')
			self.entry_field = wx.TextCtrl(self, wx.ID_ANY)
			self.Bind(wx.EVT_TEXT, self.text_entry_changed, self.entry_field)
			self.entry_field.Bind(wx.EVT_CHAR, self.text_entry_key)
			self.entry_field.SetValue(save_name)
			self.entry_sizer = wx.BoxSizer(wx.HORIZONTAL)
			self.entry_sizer.Add(self.entry_label, flag=wx.ALIGN_CENTRE_VERTICAL)
			self.entry_sizer.Add(self.entry_field, proportion=1, flag=wx.EXPAND | wx.LEFT, border=16)
			self.topsizer.Add(self.entry_sizer, flag=wx.LEFT | wx.RIGHT | wx.TOP | wx.EXPAND, border=16)
			self.entry_field.SetFocus()
			if '.' in save_name and not select_suffix:
				endsel = save_name.rindex('.')
			else:
				endsel = len(save_name)
			self.entry_field.SetSelection(0, endsel)
		if self.saving:
			self.confirm_button.SetLabel('保存')
		self.confirm_button.SetDefault()
		self.Bind(wx.EVT_BUTTON, self.open_pressed, self.confirm_button)
		self.butsizer = wx.BoxSizer(wx.HORIZONTAL)
		self.filter = filters[0]
		if self.saving:
			self.new_button = wx.Button(self, wx.ID_ANY, label= '新建文件夹')
			self.Bind(wx.EVT_BUTTON, self.new_pressed, self.new_button)
			self.butsizer.Add(self.new_button, proportion=0, flag=wx.RIGHT, border=16)
		elif len(filters) > 1:
			self.filter_choice = wx.Choice(self, wx.ID_ANY, choices=[ f.label for f in filters ])
			self.filter_choice.SetSelection(0)
			self.Bind(wx.EVT_CHOICE, self.filter_changed, self.filter_choice)
			self.butsizer.Add(self.filter_choice, proportion=0, flag=wx.RIGHT, border=16)
		self.butsizer.AddStretchSpacer()
		if mswindows:
			self.butsizer.Add(self.confirm_button, proportion=0, flag=wx.RIGHT, border=16)
			self.butsizer.Add(self.cancel_button, proportion=0)
		else:
			self.butsizer.Add(self.cancel_button, proportion=0, flag=wx.RIGHT, border=16)
			self.butsizer.Add(self.confirm_button, proportion=0)
		self.topsizer.Add(self.butsizer, proportion=0, flag=wx.ALL | wx.EXPAND, border=16)
		self.enter_normal_dir(starting_dir)
		self.SetSizerAndFit(self.topsizer)
		if not self.saving:
			self.lbox.SetFocus()
		return

	def text_entry_changed(self, e):
		val = self.entry_field.GetValue()
		self.confirm_button.Enable(val != '')

	def text_entry_key(self, e):
		key = e.GetKeyCode()
		if key < wx.WXK_SPACE or key == wx.WXK_DELETE or key > 255 or allowed_filename_char(chr(key)):
			e.Skip()

	def error_message(self, msg):
		dlg = wx.MessageDialog(self, msg, style=wx.CANCEL | wx.ICON_EXCLAMATION)
		dlg.ShowModal()
		dlg.Destroy

	def new_pressed(self, e):
		name = self.get_new_folder_name()
		if name != None:
			path = os.path.join(self.dir, name)
			if os.path.exists(path):
				self.already_exists(name)
			else:
				try:
					os.mkdir(path)
				except EnvironmentError as e:
					self.error_message('Error creating directory:\n'+ str(e))
					return

				self.enter_normal_dir(self.dir)
		return

	def get_new_folder_name(self):
		dlg = wx.TextEntryDialog(self, 'Create folder named:', 'New folder', style=wx.OK | wx.CANCEL)
		ret = dlg.ShowModal()
		val = dlg.GetValue()
		if ret == wx.ID_OK:
			return val
		else:
			return None

	def filter_changed(self, e):
		idx = e.GetInt()
		new_filter = self.filters[idx]
		if new_filter != self.filter:
			self.filter = new_filter
			self.redisplay_dir_contents()

	def item_selected(self, e):
		idx = e.GetIndex()
		it = self.items[idx]
		self.current_item = it
		if self.saving:
			if it.openable_dir():
				pass
			else:
				self.entry_field.SetValue(it.name)
		else:
			self.confirm_button.Enable()

	def item_deselected(self, e):
		self.current_item = None
		if not self.saving:
			self.confirm_button.Disable()
		return

	def item_activated(self, e):
		idx = e.GetIndex()
		self.activate(self.items[idx])

	def confirm_overwrite(self, name):
		dlg = wx.MessageDialog(self, '"%s" 已经存在，是否覆盖?' % name, '快照已存在', wx.YES_NO | wx.NO_DEFAULT | wx.ICON_QUESTION)
		ret = dlg.ShowModal()
		dlg.Destroy
		return ret == wx.ID_YES

	def already_exists(self, name):
		dlg = wx.MessageDialog(self, '"%s" 已经存在，无法覆盖.' % name, '快照已存在', wx.CANCEL | wx.ICON_EXCLAMATION)
		dlg.ShowModal()
		dlg.Destroy

	def name_change(self, sel):
		if sel.endswith(".ckpt"):
			return sel
		else:
			return ''.join([sel, ".ckpt"])

	def end_dialogue(self, name):
		if not self.dir:
			return
		sel = os.path.join(self.dir, name)
		if self.saving:
			if os.path.exists(sel):
				if self.allow_overwrite:
					if not self.confirm_overwrite(name):
						return
				else:
					self.already_exists(name)
					return
			sel = self.name_change(sel)
		print (sel)
		self.selection = sel
		self.EndModal(wx.ID_OPEN)

	def open_pressed(self, e):
		if self.saving:
			name = self.entry_field.GetValue()
			if name:
				self.end_dialogue(name)
		else:
			self.activate(self.current_item)

	def activate(self, it):
		if it:
			if it.openable_dir():
				self.enter_normal_dir(os.path.join(self.dir, it.name))
			elif not self.saving:
				self.end_dialogue(it.name)

	def enter_dir_idx(self, idx):
		if idx == len(self.dircomps):
			#self.enter_drive_dir()
			print ("enter_drive_dir")
		else:
			comps = self.dircomps[idx:]
			self.enter_normal_dir(os.path.join(*reversed(comps)))

	def dir_selected(self, e):
		idx = e.GetInt()
		if idx > 0:
			self.enter_dir_idx(idx)

	def up_pressed(self, e):
		self.enter_dir_idx(1)

	def enter_normal_dir(self, path):
		self.dir = path
		self.dircomps = list(reversed(path_components(self.dir)))
		self.dir_contents = os.listdir(self.dir)
		self.enter_dir()

	'''
	def enter_drive_dir(self):
		self.dir = ''
		self.dircomps = []
		roots = [ d + ':\\' for d in drive_letters() ]
		self.dir_contents = [ r for r in roots if os.path.isdir(r) ]
		self.enter_dir()
	'''

	def enter_dir(self):
		self.choice.Clear()
		if True:
			for s in self.dircomps:
				self.choice.Append(s)
		else:
			for i in xrange(len(self.dircomps)):
				self.choice.Append(os.path.join(*reversed(self.dircomps[i:])))

		self.choice.SetSelection(0)
		self.up_button.Enable(self.choice.GetCount() > 1)
		self.redisplay_dir_contents()

	def redisplay_dir_contents(self):
		items = [ Item(self.dir, f) for f in self.dir_contents ]
		self.items = sorted((it for it in items if it.visible() and (it.openable_dir() or self.filter.accept(it))), key=lambda it: it.sortkey())
		self.lbox.DeleteAllItems()
		for i, it in enumerate(self.items):
			self.lbox.InsertItem(i, '')
			self.lbox.SetItemImage(i, self.icons[it.icon()])
			self.lbox.SetItem(i, 1, it.name)
			if self.saving and not it.openable_dir():
				self.lbox.SetItemTextColour(i, '#666666')

		self.item_deselected(None)
		return


def select_file(app, title, dir, **kw):
	""" Select a file to open or save. Return the full path name, or None if
	the operation was cancelled."""
	if not os.path.isdir(dir):
		dir = os.path.abspath('.')
	dlg = File_selector(app, title, dir, **kw)
	val = dlg.ShowModal()
	dlg.Destroy()
	if val == wx.ID_CANCEL:
		return None
	else:
		return dlg.selection


def main():
	app = wx.PySimpleApp()
	frame = wx.Frame(None, wx.ID_ANY, 'fsel demo')
	sizer = wx.BoxSizer(wx.HORIZONTAL)
	but1 = wx.Button(frame, wx.ID_ANY, 'open')
	sizer.Add(but1)
	but2 = wx.Button(frame, wx.ID_ANY, 'save')
	sizer.Add(but2)
	frame.SetSizerAndFit(sizer)

	def select_open(app, evt):
		f = select_file(app, 'Please open vely nice file', os.getcwdu(), filters=[Filter_checkpoint(), Filter_all()])
		print ('selected file:', repr(f))

	def select_save(app, evt):
		f = select_file(app, 'Saving something!', os.getcwdu(), save_name='skyeye.ckpt', allow_overwrite=False)
		print ('selected file:', repr(f))

	frame.Bind(wx.EVT_BUTTON, lambda e: select_open(frame, e), but1)
	frame.Bind(wx.EVT_BUTTON, lambda e: select_save(frame, e), but2)
	frame.Show(True)
	app.MainLoop()
	return


if __name__ == '__main__':
	main()
