using System;
using System.Globalization;
using System.Windows;
using System.Windows.Data;

namespace FCSVisualChart.Converter
{
    public class BoolToVisibilityConverter : IValueConverter
    {
        internal static BoolToVisibilityConverter Converter { get; } = new BoolToVisibilityConverter();
        public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
        {
            if (value is bool b)
            {
                if (parameter is bool p && p) b = !b;
                return b ? Visibility.Visible : Visibility.Collapsed;
            }
            return value;
        }

        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        {
            if (value is Visibility b)
            {
                var temp = b == Visibility.Visible;
                return (parameter is bool p && p) ? !temp : temp;
            }
            return value;
        }
    }
}
