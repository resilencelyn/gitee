using System;
using System.Reflection;

namespace Mozi.IoT.Generic
{
    /// <summary>
    /// ��ö�� ������
    /// </summary>
    public abstract class AbsClassEnum
    {
        protected abstract string Tag { get; }
        /// <summary>
        /// ��ȡ���� �����ֱ�ʶ����Сд
        /// </summary>
        /// <param name="name"></param>
        /// <returns></returns>
        public static T Get<T>(string name) where T : AbsClassEnum
        {
            //T t = Activator.CreateInstance<T>();
            FieldInfo[] pis = typeof(T).GetFields(BindingFlags.IgnoreCase | BindingFlags.Public | BindingFlags.Static);
            foreach (var info in pis)
            {
                object oc = info.GetValue(null);
                if (oc != null)
                {
                    if (((T)oc).Tag.Equals(name, StringComparison.OrdinalIgnoreCase))
                    {
                        return (T)oc;
                    };
                }
            }
            return null;
        }
        /// <summary>
        /// �˴��жϱ�ʶ���Ƿ����,���ִ�Сд
        /// <para>
        ///     ���Ҫ�ж��Ӷ����Ƿ����<see cref="null"/>����ʹ��<see cref="object.Equals(object, object)"/>
        /// </para>
        /// </summary>
        /// <param name="obj"></param>
        /// <returns></returns>
        public override bool Equals(object obj)
        {
            return obj is AbsClassEnum && ((AbsClassEnum)obj).Tag.Equals(Tag);
        }
        /// <summary>
        /// ����==
        /// <para>
        ///     ���Ҫ�ж��Ӷ����Ƿ����<see cref="null"/>����ʹ��<see cref="object.Equals(object, object)"/>
        /// </para>
        /// </summary>
        /// <param name="a"></param>
        /// <param name="b"></param>
        /// <returns></returns>
        public static bool operator ==(AbsClassEnum a, AbsClassEnum b)
        {
            return (object)b != null && (object)a != null && a.Tag.Equals(b.Tag);
        }

        /// <summary>
        /// ����!=
        /// <para>
        ///     ���Ҫ�ж��Ӷ����Ƿ����<see cref="null"/>����ʹ��<see cref="object.Equals(object, object)"/>
        /// </para>
        /// </summary>
        /// <param name="a"></param>
        /// <param name="b"></param>
        /// <returns></returns>
        public static bool operator !=(AbsClassEnum a, AbsClassEnum b)
        {
            return (object)a == null || (object)b == null || !a.Tag.Equals(b.Tag);
        }

        public override int GetHashCode()
        {
            return Tag.GetHashCode();
        }
    }
}