using System.Collections;
using System.Collections.Generic;

namespace Mozi.HttpEmbedded
{
    /// <summary>
    /// �ϴ����ļ�
    /// </summary>
    public class File
    {
        public string FileName { get; set; }
        public string FieldName { get; set; }
        public int FileIndex { get; set; }
        public byte[] FileData { get; set; }
        public string FileTempSavePath { get; set; }
        ~File()
        {
            FileData = null;
        }
    }
    /// <summary>
    /// �ļ�����
    /// </summary>
    public class FileCollection:IEnumerable
    {
        private readonly List<File> _files = new List<File>();

        public File this[string name] { get { return GetItem(name); } }
        public File this[int ind] { get { return _files[ind]; } }

        /// <summary>
        /// �ļ�����
        /// </summary>
        public List<File> Files { get { return _files; } }
        /// <summary>
        /// ���ϼ���
        /// </summary>
        public int Count { get { return _files.Count; } }

        public IEnumerator GetEnumerator()
        {
            return new FileCollectionEnumerator(_files);
        }
        /// <summary>
        /// ��ȡ��
        /// </summary>
        /// <param name="name"></param>
        /// <returns></returns>
        public File GetItem(string name)
        {
            return _files.Find(x => x.FileName.Equals(name));
        }
        /// <summary>
        /// ������
        /// </summary>
        /// <param name="f"></param>
        public void Add(File item)
        {
            _files.Add(item);
        }
        /// <summary>
        /// ����������Ŀ
        /// </summary>
        /// <param name="items"></param>
        public void AddRange(IEnumerable<File> items)
        {
            _files.AddRange(items);
        }
        /// <summary>
        /// ��ռ���
        /// </summary>
        public void Clear()
        {
            _files.Clear();
        }
        /// <summary>
        /// �Ƴ���
        /// </summary>
        /// <param name="item"></param>
        /// <returns></returns>
        public bool Remove(File item)
        {
            return _files.Remove(item);
        }
        /// <summary>
        /// �Ƴ�ָ��λ�õ���
        /// </summary>
        /// <param name="ind"></param>
        public void RemoveAt(int ind)
        {
            _files.RemoveAt(ind);
        }
        /// <summary>
        /// ��ȡ�������λ��
        /// </summary>
        /// <param name="item"></param>
        /// <returns></returns>
        public int IndexOf(File item)
        {
            return _files.IndexOf(item);
        }
    }
    /// <summary>
    /// �ļ����ϵ�����
    /// </summary>
    public class FileCollectionEnumerator : IEnumerator
    {
        private int _index;
        private List<File> _collection;
        private File value;
        public FileCollectionEnumerator(List<File> colletion)
        {
            _collection = colletion;
            _index = -1;
        }
        object IEnumerator.Current
        {
            get { return value; }
        }

        public bool MoveNext()
        {
            _index++;
            if (_index >= _collection.Count)
            {
                return false;
            }
            else
            {
                value = _collection[_index]; 
            }
            return true;
        }
        public void Reset()
        {
            _index = -1;
        }
    }
}