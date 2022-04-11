using Android.Content;
using Android.Views;
using astator.Core.Script;
using astator.Core.UI.Base;
using astator.Modules;
using Microsoft.Maui.Platform;

namespace astator.Views
{
    public partial class PathCard : CustomCard
    {
        public static readonly BindableProperty TypeImageSourceBindableProperty = BindableProperty.Create(nameof(TypeImageSource), typeof(string), typeof(PathCard));
        public string TypeImageSource
        {
            get => GetValue(TypeImageSourceBindableProperty) as string;
            set => SetValue(TypeImageSourceBindableProperty, value);
        }

        public static readonly BindableProperty MenuImageSourceBindableProperty = BindableProperty.Create(nameof(MenuImageSource), typeof(string), typeof(PathCard));
        public string MenuImageSource
        {
            get => GetValue(MenuImageSourceBindableProperty) as string;
            set => SetValue(MenuImageSourceBindableProperty, value);
        }

        public static readonly BindableProperty PathNameBindableProperty = BindableProperty.Create(nameof(PathName), typeof(string), typeof(PathCard));
        public string PathName
        {
            get => GetValue(PathNameBindableProperty) as string;
            set => SetValue(PathNameBindableProperty, value);
        }

        public static readonly BindableProperty PathInfoBindableProperty = BindableProperty.Create(nameof(PathInfo), typeof(string), typeof(PathCard));
        public string PathInfo
        {
            get => GetValue(PathInfoBindableProperty) as string;
            set => SetValue(PathInfoBindableProperty, value);
        }

        public static readonly BindableProperty IsAddMenuBindableProperty = BindableProperty.Create(nameof(IsAddMenu), typeof(bool), typeof(PathCard));
        public bool IsAddMenu
        {
            get => (bool)GetValue(IsAddMenuBindableProperty);
            set => SetValue(IsAddMenuBindableProperty, value);
        }

        private bool AlreadyAddMenu = false;

        public PathCard()
        {
            InitializeComponent();
        }

        protected override void OnHandlerChanged()
        {
            base.OnHandlerChanged();

            if (this.IsAddMenu && !this.AlreadyAddMenu)
            {
                this.AlreadyAddMenu = true;
                var view = this.Handler.PlatformView as LayoutViewGroup;
                var menu = new AndroidX.AppCompat.Widget.PopupMenu(Globals.AppContext, view, (int)GravityFlags.Right);
                if (this.PathName.EndsWith(".csproj"))
                {
                    menu.Menu.Add("������Ŀ");
                    menu.Menu.Add("���apk");
                    menu.Menu.Add("����dll");
                }
                else if (this.PathName.EndsWith(".cs"))
                {
                    menu.Menu.Add("���нű�");
                }

                menu.Menu.Add("������");
                menu.Menu.Add("ɾ��");
                menu.Menu.Add("����");
                menu.Menu.Add("����Ӧ�ô�");

                menu.SetOnMenuItemClickListener(new OnMenuItemClickListener((item) =>
                {
                    if (item.TitleFormatted.ToString() == "������Ŀ")
                    {
                        _ = ScriptManager.Instance.RunProject(Path.GetDirectoryName(this.Tag.ToString()));
                    }
                    else if (item.TitleFormatted.ToString() == "���нű�")
                    {
                        _ = ScriptManager.Instance.RunScript(this.Tag.ToString());
                    }
                    else if (item.TitleFormatted.ToString() == "����Ӧ�ô�")
                    {
                        var path = this.Tag.ToString();
                        var intent = new Intent(Intent.ActionView);
                        intent.AddFlags(ActivityFlags.NewTask);
                        var contentType = new FileResult(this.Tag.ToString()).ContentType;
                        var uri = AndroidX.Core.Content.FileProvider.GetUriForFile(Android.App.Application.Context,
                            Android.App.Application.Context.PackageName + ".fileProvider",
                            new Java.IO.File(path));
                        intent.AddFlags(ActivityFlags.GrantReadUriPermission | ActivityFlags.GrantWriteUriPermission);
                        intent.SetDataAndType(uri, contentType);

                        Globals.AppContext.StartActivity(intent);
                    }
                    else if (item.TitleFormatted.ToString() == "������")
                    {
                        var title = new Label
                        {
                            Text = "�������ļ�",
                            FontSize = 20,
                            FontAttributes = FontAttributes.Bold,
                            HorizontalOptions = LayoutOptions.Start,
                            VerticalOptions = LayoutOptions.Center
                        };

                        var editor = new Editor
                        {
                            FontSize = 16,
                            Placeholder = "�������ļ���"
                        };

                        var cancel = new CustomLabelButton
                        {
                            Text = "ȡ��",
                            Padding = new Thickness(16, 10),
                            BackgroundColor = (Color)Application.Current.Resources["PrimaryColor"],
                            FontSize = 15
                        };

                        var confirm = new CustomLabelButton
                        {
                            Text = "ȷ��",
                            Margin = new Thickness(5, 0, 0, 0),
                            Padding = new Thickness(16, 10),
                            BackgroundColor = (Color)Application.Current.Resources["PrimaryColor"],
                            FontSize = 15
                        };
                        var stack = new HorizontalStackLayout
                        {
                            cancel,
                            confirm
                        };
                        stack.HorizontalOptions = LayoutOptions.End;

                        var layout = new Grid
                        {
                            title,
                            editor,
                            stack
                        };

                        layout.Padding = new Thickness(20, 15, 20, 5);
                        layout.RowSpacing = 10;
                        layout.RowDefinitions = new RowDefinitionCollection
                        {
                            new RowDefinition { Height = GridLength.Auto },
                            new RowDefinition { Height = GridLength.Auto },
                            new RowDefinition { Height = GridLength.Auto }
                        };

                        title.SetValue(RowProperty, 0);
                        editor.SetValue(RowProperty, 1);
                        stack.SetValue(RowProperty, 2);

                        var view = layout.ToPlatform(this.Handler.MauiContext);
                        var builder = new AndroidX.AppCompat.App.AlertDialog.Builder(Globals.AppContext).SetView(view);
                        var dialog = builder.Show();

                        cancel.Clicked += (s, e) =>
                        {
                            dialog.Dismiss();
                        };

                        confirm.Clicked += (s, e) =>
                        {
                            dialog.Dismiss();
                            var oldName = this.Tag.ToString();
                            var newName = Path.Combine(Path.GetDirectoryName(oldName), editor.Text);
                            File.Move(oldName, newName);
                            var refreshView = this.Parent?.Parent?.Parent?.Parent as RefreshView;
                            if (refreshView is not null)
                            {
                                refreshView.IsRefreshing = true;
                            }
                        };
                    }
                    else if (item.TitleFormatted.ToString() == "ɾ��")
                    {
                        var alert = new AndroidX.AppCompat.App.AlertDialog
                            .Builder(Globals.AppContext)
                            .SetTitle("ɾ���ļ�")
                            .SetMessage($"ȷ��ɾ�� \"{this.Tag}\" ��?")
                            .SetPositiveButton("ȷ��", (s, e) =>
                            {
                                File.Delete(this.Tag.ToString());
                                var refreshView = this.Parent?.Parent?.Parent?.Parent as RefreshView;
                                if (refreshView is not null)
                                {
                                    refreshView.IsRefreshing = true;
                                }
                            })
                            .SetNegativeButton("ȡ��", (s, e) => { });

                        alert.Show();
                    }
                    else if (item.TitleFormatted.ToString() == "����")
                    {
                        var path = this.Tag.ToString();
                        var intent = new Intent(Intent.ActionSend);
                        var contentType = new FileResult(this.Tag.ToString()).ContentType;
                        var uri = AndroidX.Core.Content.FileProvider.GetUriForFile(Android.App.Application.Context,
                            Android.App.Application.Context.PackageName + ".fileProvider",
                            new Java.IO.File(path));
                        intent.SetType(contentType);
                        intent.PutExtra(Intent.ExtraStream, uri);
                        intent.AddFlags(ActivityFlags.GrantReadUriPermission);
                        Globals.AppContext.StartActivity(intent);
                    }
                    else if (item.TitleFormatted.ToString() == "���apk")
                    {
                        var path = this.Tag.ToString();
                        var apkbuilderer = new ApkBuilderer(Path.GetDirectoryName(path));
                        _ = apkbuilderer.Build();
                    }
                    else if (item.TitleFormatted.ToString() == "����dll")
                    {

                        var path = this.Tag.ToString();
                        var apkbuilderer = new ApkBuilderer(Path.GetDirectoryName(path));
                        _ = apkbuilderer.CompileDll();
                    }

                    return true;
                }));

                view.SetOnLongClickListener(new OnLongClickListener((v) =>
                {
                    menu.Show();
                    return true;
                }));
            }
        }
    }
}

