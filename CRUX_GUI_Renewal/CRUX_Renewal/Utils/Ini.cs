﻿using CRUX_Renewal;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

public struct IniValue {
    private static bool TryParseInt(string text, out int value) 
    {
        try
        {
            int res;
            if (Int32.TryParse(text,
                System.Globalization.NumberStyles.Integer,
                System.Globalization.CultureInfo.InvariantCulture,
                out res))
            {
                value = res;
                return true;
            }
            value = 0;
            return false;
        }
        catch (Exception ex)
        {
            Systems.LogWriter.Error(string.Format("Exception Message : {0} Stack : {1} Prev Func Name : {2} }", ex.Message.ToString(), ex.StackTrace.ToString(), $"{new StackFrame(1, true).GetMethod().Name}"));
            throw ex;
        }
    }

    private static bool TryParseDouble(string text, out double value)
    {
        try
        {
            double res;
            if (Double.TryParse(text,
                System.Globalization.NumberStyles.Float,
                System.Globalization.CultureInfo.InvariantCulture,
                out res))
            {
                value = res;
                return true;
            }
            value = Double.NaN;
            return false;
        }
        catch (Exception ex)
        {
            Systems.LogWriter.Error(string.Format("Exception Message : {0} Stack : {1} Prev Func Name : {2} }", ex.Message.ToString(), ex.StackTrace.ToString(), $"{new StackFrame(1, true).GetMethod().Name}"));
            throw ex;
        }
    }

    public string Value;

    public IniValue(object value)
    {
        try
        {
            var formattable = value as IFormattable;
            if (formattable != null)
            {
                Value = formattable.ToString(null, System.Globalization.CultureInfo.InvariantCulture);
            }
            else
            {
                Value = value != null ? value.ToString() : null;
            }
        }
        catch (Exception ex)
        {
            Systems.LogWriter.Error(string.Format("Exception Message : {0} Stack : {1} Prev Func Name : {2} }", ex.Message.ToString(), ex.StackTrace.ToString(), $"{new StackFrame(1, true).GetMethod().Name}"));
            throw ex;
        }
    }

    public IniValue(string value)
    {
        Value = value;
    }

    public bool ToBool(bool valueIfInvalid = false)
    {
        try
        {
            bool res;
            if (TryConvertBool(out res))
            {
                return res;
            }
            return valueIfInvalid;
        }
        catch (Exception ex)
        {
            Systems.LogWriter.Error(string.Format("Exception Message : {0} Stack : {1} Prev Func Name : {2} }", ex.Message.ToString(), ex.StackTrace.ToString(), $"{new StackFrame(1, true).GetMethod().Name}"));
            throw ex;
        }
    }

    public bool TryConvertBool(out bool result) 
    {
        try
        {
            if (Value == null)
            {
                result = default(bool);
                return false;
            }
            var boolStr = Value.Trim().ToLowerInvariant();
            if (boolStr == "true")
            {
                result = true;
                return true;
            }
            else if (boolStr == "false")
            {
                result = false;
                return true;
            }
            result = default(bool);
            return false;
        }
        catch (Exception ex)
        {
            Systems.LogWriter.Error(string.Format("Exception Message : {0} Stack : {1} Prev Func Name : {2} }", ex.Message.ToString(), ex.StackTrace.ToString(), $"{new StackFrame(1, true).GetMethod().Name}"));
            throw ex;
        }
    }

    public int ToInt(int valueIfInvalid = 0) 
    {
        try
        {
            int res;
            if (TryConvertInt(out res))
            {
                return res;
            }
            return valueIfInvalid;
        }
        catch (Exception ex)
        {
            Systems.LogWriter.Error(string.Format("Exception Message : {0} Stack : {1} Prev Func Name : {2} }", ex.Message.ToString(), ex.StackTrace.ToString(), $"{new StackFrame(1, true).GetMethod().Name}"));
            throw ex;
        }
    }

    public bool TryConvertInt(out int result)
    {
        try
        {
            if (Value == null)
            {
                result = default(int);
                return false;
            }
            if (TryParseInt(Value.Trim(), out result))
            {
                return true;
            }
            return false;
        }
        catch (Exception ex)
        {
            Systems.LogWriter.Error(string.Format("Exception Message : {0} Stack : {1} Prev Func Name : {2} }", ex.Message.ToString(), ex.StackTrace.ToString(), $"{new StackFrame(1, true).GetMethod().Name}"));
            throw ex;
        }
    }

    public double ToDouble(double valueIfInvalid = 0) 
    {
        try
        {
            double res;
            if (TryConvertDouble(out res))
            {
                return res;
            }
            return valueIfInvalid;
        }
        catch (Exception ex)
        {
            Systems.LogWriter.Error(string.Format("Exception Message : {0} Stack : {1} Prev Func Name : {2} }", ex.Message.ToString(), ex.StackTrace.ToString(), $"{new StackFrame(1, true).GetMethod().Name}"));
            throw ex;
        }
    }

    public bool TryConvertDouble(out double result)
    {
        try
        {
            if (Value == null)
            {
                result = default(double);
                return false; ;
            }
            if (TryParseDouble(Value.Trim(), out result))
            {
                return true;
            }
            return false;
        }
        catch (Exception ex)
        {
            Systems.LogWriter.Error(string.Format("Exception Message : {0} Stack : {1} Prev Func Name : {2} }", ex.Message.ToString(), ex.StackTrace.ToString(), $"{new StackFrame(1, true).GetMethod().Name}"));
            throw ex;
        }
    }

    public string GetString() 
    {
        try
        {
            return GetString(true, false);
        }
        catch (Exception ex)
        {
            Systems.LogWriter.Error(string.Format("Exception Message : {0} Stack : {1} Prev Func Name : {2} }", ex.Message.ToString(), ex.StackTrace.ToString(), $"{new StackFrame(1, true).GetMethod().Name}"));
            throw ex;
        }
    }

    public string GetString(bool preserveWhitespace)
    {
        try
        {
            return GetString(true, preserveWhitespace);
        }
        catch (Exception ex)
        {
            Systems.LogWriter.Error(string.Format("Exception Message : {0} Stack : {1} Prev Func Name : {2} }", ex.Message.ToString(), ex.StackTrace.ToString(), $"{new StackFrame(1, true).GetMethod().Name}"));
            throw ex;
        }
    }

    public string GetString(bool allowOuterQuotes, bool preserveWhitespace) 
    {
        try
        {
            if (Value == null)
            {
                return "";
            }
            var trimmed = Value.Trim();
            if (allowOuterQuotes && trimmed.Length >= 2 && trimmed[0] == '"' && trimmed[trimmed.Length - 1] == '"')
            {
                var inner = trimmed.Substring(1, trimmed.Length - 2);
                return preserveWhitespace ? inner : inner.Trim();
            }
            else
            {
                return preserveWhitespace ? Value : Value.Trim();
            }
        }
        catch (Exception ex)
        {
            Systems.LogWriter.Error(string.Format("Exception Message : {0} Stack : {1} Prev Func Name : {2} }", ex.Message.ToString(), ex.StackTrace.ToString(), $"{new StackFrame(1, true).GetMethod().Name}"));
            throw ex;
        }
    }

    public override string ToString() {
        return Value;
    }

    public static implicit operator IniValue(byte o) {
        return new IniValue(o);
    }

    public static implicit operator IniValue(short o) {
        return new IniValue(o);
    }

    public static implicit operator IniValue(int o) {
        return new IniValue(o);
    }

    public static implicit operator IniValue(sbyte o) {
        return new IniValue(o);
    }

    public static implicit operator IniValue(ushort o) {
        return new IniValue(o);
    }

    public static implicit operator IniValue(uint o) {
        return new IniValue(o);
    }

    public static implicit operator IniValue(float o) {
        return new IniValue(o);
    }

    public static implicit operator IniValue(double o) {
        return new IniValue(o);
    }

    public static implicit operator IniValue(bool o) {
        return new IniValue(o);
    }

    public static implicit operator IniValue(string o) {
        return new IniValue(o);
    }

    private static readonly IniValue _default = new IniValue();
    public static IniValue Default { get { return _default; } }
}

public class IniFile : IEnumerable<KeyValuePair<string, IniSection>>, IDictionary<string, IniSection> {
    private Dictionary<string, IniSection> sections;
    public IEqualityComparer<string> StringComparer;

    public bool SaveEmptySections;

    public IniFile() 
        : this(DefaultComparer) {
    }

    public IniFile(IEqualityComparer<string> stringComparer) 
    {
        try
        {
            StringComparer = stringComparer;
            sections = new Dictionary<string, IniSection>(StringComparer);
        }
        catch (Exception ex)
        {
            Systems.LogWriter.Error(string.Format("Exception Message : {0} Stack : {1} Prev Func Name : {2} }", ex.Message.ToString(), ex.StackTrace.ToString(), $"{new StackFrame(1, true).GetMethod().Name}"));
            throw ex;
        }
    }

    public void Save(string path, FileMode mode = FileMode.Create)
    {
        try
        {
            using (var stream = new FileStream(path, mode, FileAccess.Write))
            {
                Save(stream);
            }
        }
        catch (Exception ex)
        {
            Systems.LogWriter.Error(string.Format("Exception Message : {0} Stack : {1} Prev Func Name : {2} }", ex.Message.ToString(), ex.StackTrace.ToString(), $"{new StackFrame(1, true).GetMethod().Name}"));
            throw ex;
        }
    }

    public void Save(Stream stream) 
    {
        try
        {
            using (var writer = new StreamWriter(stream))
            {
                Save(writer);
            }
        }
        catch (Exception ex)
        {
            Systems.LogWriter.Error(string.Format("Exception Message : {0} Stack : {1} Prev Func Name : {2} }", ex.Message.ToString(), ex.StackTrace.ToString(), $"{new StackFrame(1, true).GetMethod().Name}"));
            throw ex;
        }
    }

    public void Save(StreamWriter writer) 
    {
        try
        {
            foreach (var section in sections)
            {
                if (section.Value.Count > 0 || SaveEmptySections)
                {
                    writer.WriteLine(string.Format("[{0}]", section.Key.Trim()));
                    foreach (var kvp in section.Value)
                    {
                        writer.WriteLine(string.Format("{0}={1}", kvp.Key, kvp.Value));
                    }
                    writer.WriteLine("");
                }
            }
        }
        catch (Exception ex)
        {
            Systems.LogWriter.Error(string.Format("Exception Message : {0} Stack : {1} Prev Func Name : {2} }", ex.Message.ToString(), ex.StackTrace.ToString(), $"{new StackFrame(1, true).GetMethod().Name}"));
            throw ex;
        }
    }

    public void Load(string path, bool ordered = false) 
    {
        try
        {
            using (var stream = new FileStream(path, FileMode.Open, FileAccess.Read))
            {
                Load(stream, ordered);
            }
        }
        catch (Exception ex)
        {
            Systems.LogWriter.Error(string.Format("Exception Message : {0} Stack : {1} Prev Func Name : {2} }", ex.Message.ToString(), ex.StackTrace.ToString(), $"{new StackFrame(1, true).GetMethod().Name}"));
            throw ex;
        }
    }

    public void Load(Stream stream, bool ordered = false)
    {
        try
        {
            using (var reader = new StreamReader(stream))
            {
                Load(reader, ordered);
            }
        }
        catch (Exception ex)
        {
            Systems.LogWriter.Error(string.Format("Exception Message : {0} Stack : {1} Prev Func Name : {2} }", ex.Message.ToString(), ex.StackTrace.ToString(), $"{new StackFrame(1, true).GetMethod().Name}"));
            throw ex;
        }
    }

    public void Load(StreamReader reader, bool ordered = false)
    {
        try
        {
            IniSection section = null;

            while (!reader.EndOfStream)
            {
                var line = reader.ReadLine();

                if (line != null)
                {
                    var trimStart = line.TrimStart();

                    if (trimStart.Length > 0)
                    {
                        if (trimStart[0] == '[')
                        {
                            var sectionEnd = trimStart.IndexOf(']');
                            if (sectionEnd > 0)
                            {
                                var sectionName = trimStart.Substring(1, sectionEnd - 1).Trim();
                                section = new IniSection(StringComparer) { Ordered = ordered };
                                sections[sectionName] = section;
                            }
                        }
                        else if (section != null && trimStart[0] != ';')
                        {
                            string key;
                            IniValue val;

                            if (LoadValue(line, out key, out val))
                            {
                                section[key] = val;
                            }
                        }
                    }
                }
            }
        }
        catch (Exception ex)
        {
            Systems.LogWriter.Error(string.Format("Exception Message : {0} Stack : {1} Prev Func Name : {2} }", ex.Message.ToString(), ex.StackTrace.ToString(), $"{new StackFrame(1, true).GetMethod().Name}"));
            throw ex;
        }
    }

    private bool LoadValue(string line, out string key, out IniValue val)
    {
        try
        {
            var assignIndex = line.IndexOf('=');
            if (assignIndex <= 0)
            {
                key = null;
                val = null;
                return false;
            }

            key = line.Substring(0, assignIndex).Trim();
            var value = line.Substring(assignIndex + 1);

            val = new IniValue(value);
            return true;
        }
        catch (Exception ex)
        {
            Systems.LogWriter.Error(string.Format("Exception Message : {0} Stack : {1} Prev Func Name : {2} }", ex.Message.ToString(), ex.StackTrace.ToString(), $"{new StackFrame(1, true).GetMethod().Name}"));
            throw ex;
        }
    }

    public bool ContainsSection(string section) 
    {
        try
        {
            return sections.ContainsKey(section);
        }
        catch (Exception ex)
        {
            Systems.LogWriter.Error(string.Format("Exception Message : {0} Stack : {1} Prev Func Name : {2} }", ex.Message.ToString(), ex.StackTrace.ToString(), $"{new StackFrame(1, true).GetMethod().Name}"));
            throw ex;
        }
    }

    public bool TryGetSection(string section, out IniSection result) 
    {
        try
        {
            return sections.TryGetValue(section, out result);
        }
        catch (Exception ex)
        {
            Systems.LogWriter.Error(string.Format("Exception Message : {0} Stack : {1} Prev Func Name : {2} }", ex.Message.ToString(), ex.StackTrace.ToString(), $"{new StackFrame(1, true).GetMethod().Name}"));
            throw ex;
        }
    }

    bool IDictionary<string, IniSection>.TryGetValue(string key, out IniSection value) 
    {
        try
        {
            return TryGetSection(key, out value);
        }
        catch (Exception ex)
        {
            Systems.LogWriter.Error(string.Format("Exception Message : {0} Stack : {1} Prev Func Name : {2} }", ex.Message.ToString(), ex.StackTrace.ToString(), $"{new StackFrame(1, true).GetMethod().Name}"));
            throw ex;
        }
    }

    public bool Remove(string section) 
    {
        try
        {
            return sections.Remove(section);
        }
        catch (Exception ex)
        {
            Systems.LogWriter.Error(string.Format("Exception Message : {0} Stack : {1} Prev Func Name : {2} }", ex.Message.ToString(), ex.StackTrace.ToString(), $"{new StackFrame(1, true).GetMethod().Name}"));
            throw ex;
        }
    }

    public IniSection Add(string section, Dictionary<string, IniValue> values, bool ordered = false)
    {
        try
        {
            return Add(section, new IniSection(values, StringComparer) { Ordered = ordered });
        }
        catch (Exception ex)
        {
            Systems.LogWriter.Error(string.Format("Exception Message : {0} Stack : {1} Prev Func Name : {2} }", ex.Message.ToString(), ex.StackTrace.ToString(), $"{new StackFrame(1, true).GetMethod().Name}"));
            throw ex;
        }
    }

    public IniSection Add(string section, IniSection value) 
    {
        try
        {
            if (value.Comparer != StringComparer)
            {
                value = new IniSection(value, StringComparer);
            }
            sections.Add(section, value);
            return value;
        }
        catch (Exception ex)
        {
            Systems.LogWriter.Error(string.Format("Exception Message : {0} Stack : {1} Prev Func Name : {2} }", ex.Message.ToString(), ex.StackTrace.ToString(), $"{new StackFrame(1, true).GetMethod().Name}"));
            throw ex;
        }
    }

    public IniSection Add(string section, bool ordered = false) 
    {
        try
        {
            var value = new IniSection(StringComparer) { Ordered = ordered };
            sections.Add(section, value);
            return value;
        }
        catch (Exception ex)
        {
            Systems.LogWriter.Error(string.Format("Exception Message : {0} Stack : {1} Prev Func Name : {2} }", ex.Message.ToString(), ex.StackTrace.ToString(), $"{new StackFrame(1, true).GetMethod().Name}"));
            throw ex;
        }
    }

    void IDictionary<string, IniSection>.Add(string key, IniSection value) 
    {
        try
        {
            Add(key, value);
        }
        catch (Exception ex)
        {
            Systems.LogWriter.Error(string.Format("Exception Message : {0} Stack : {1} Prev Func Name : {2} }", ex.Message.ToString(), ex.StackTrace.ToString(), $"{new StackFrame(1, true).GetMethod().Name}"));
            throw ex;
        }
    }

    bool IDictionary<string, IniSection>.ContainsKey(string key) 
    {
        try
        {
            return ContainsSection(key);
        }
        catch (Exception ex)
        {
            Systems.LogWriter.Error(string.Format("Exception Message : {0} Stack : {1} Prev Func Name : {2} }", ex.Message.ToString(), ex.StackTrace.ToString(), $"{new StackFrame(1, true).GetMethod().Name}"));
            throw ex;
        }
    }

    public ICollection<string> Keys 
    {
        get { return sections.Keys; }
    }

    public ICollection<IniSection> Values 
    {
        get { return sections.Values; }
    }

    void ICollection<KeyValuePair<string, IniSection>>.Add(KeyValuePair<string, IniSection> item) 
    {
        try
        {
            ((IDictionary<string, IniSection>)sections).Add(item);
        }
        catch (Exception ex)
        {
            Systems.LogWriter.Error(string.Format("Exception Message : {0} Stack : {1} Prev Func Name : {2} }", ex.Message.ToString(), ex.StackTrace.ToString(), $"{new StackFrame(1, true).GetMethod().Name}"));
            throw ex;
        }
    }

    public void Clear() 
    {
        try
        {
            sections.Clear();
        }
        catch (Exception ex)
        {
            Systems.LogWriter.Error(string.Format("Exception Message : {0} Stack : {1} Prev Func Name : {2} }", ex.Message.ToString(), ex.StackTrace.ToString(), $"{new StackFrame(1, true).GetMethod().Name}"));
            throw ex;
        }
    }

    bool ICollection<KeyValuePair<string, IniSection>>.Contains(KeyValuePair<string, IniSection> item) 
    {
        try
        {
            return ((IDictionary<string, IniSection>)sections).Contains(item);
        }
        catch (Exception ex)
        {
            Systems.LogWriter.Error(string.Format("Exception Message : {0} Stack : {1} Prev Func Name : {2} }", ex.Message.ToString(), ex.StackTrace.ToString(), $"{new StackFrame(1, true).GetMethod().Name}"));
            throw ex;
        }
    }

    void ICollection<KeyValuePair<string, IniSection>>.CopyTo(KeyValuePair<string, IniSection>[] array, int arrayIndex)
    {
        try
        {
            ((IDictionary<string, IniSection>)sections).CopyTo(array, arrayIndex);
        }
        catch (Exception ex)
        {
            Systems.LogWriter.Error(string.Format("Exception Message : {0} Stack : {1} Prev Func Name : {2} }", ex.Message.ToString(), ex.StackTrace.ToString(), $"{new StackFrame(1, true).GetMethod().Name}"));
            throw ex;
        }
    }

    public int Count {
        get { return sections.Count; }
    }

    bool ICollection<KeyValuePair<string, IniSection>>.IsReadOnly 
    {
        get { return ((IDictionary<string, IniSection>)sections).IsReadOnly; }
    }

    bool ICollection<KeyValuePair<string, IniSection>>.Remove(KeyValuePair<string, IniSection> item)
    {
        try
        {
            return ((IDictionary<string, IniSection>)sections).Remove(item);
        }
        catch (Exception ex)
        {
            Systems.LogWriter.Error(string.Format("Exception Message : {0} Stack : {1} Prev Func Name : {2} }", ex.Message.ToString(), ex.StackTrace.ToString(), $"{new StackFrame(1, true).GetMethod().Name}"));
            throw ex;
        }
    }

    public IEnumerator<KeyValuePair<string, IniSection>> GetEnumerator() 
    {
        try
        {
            return sections.GetEnumerator();
        }
        catch (Exception ex)
        {
            Systems.LogWriter.Error(string.Format("Exception Message : {0} Stack : {1} Prev Func Name : {2} }", ex.Message.ToString(), ex.StackTrace.ToString(), $"{new StackFrame(1, true).GetMethod().Name}"));
            throw ex;
        }
    }

    System.Collections.IEnumerator System.Collections.IEnumerable.GetEnumerator()
    {
        try
        {
            return GetEnumerator();
        }
        catch (Exception ex)
        {
            Systems.LogWriter.Error(string.Format("Exception Message : {0} Stack : {1} Prev Func Name : {2} }", ex.Message.ToString(), ex.StackTrace.ToString(), $"{new StackFrame(1, true).GetMethod().Name}"));
            throw ex;
        }
    }

    public IniSection this[string section] 
    {
        get {
            try
            {
                IniSection s;
                if (sections.TryGetValue(section, out s))
                {
                    return s;
                }
                s = new IniSection(StringComparer);
                sections[section] = s;
                return s;
            }
            catch (Exception ex)
            {
                Systems.LogWriter.Error(string.Format("Exception Message : {0} Stack : {1} Prev Func Name : {2} }", ex.Message.ToString(), ex.StackTrace.ToString(), $"{new StackFrame(1, true).GetMethod().Name}"));
                throw ex;
            }
        }
        set {
            try
            {
                var v = value;
                if (v.Comparer != StringComparer)
                {
                    v = new IniSection(v, StringComparer);
                }
                sections[section] = v;
            }
            catch (Exception ex)
            {
                Systems.LogWriter.Error(string.Format("Exception Message : {0} Stack : {1} Prev Func Name : {2} }", ex.Message.ToString(), ex.StackTrace.ToString(), $"{new StackFrame(1, true).GetMethod().Name}"));
                throw ex;
            }
        }
    }

    public string GetContents() 
    {
        try
        {
            using (var stream = new MemoryStream())
            {
                Save(stream);
                stream.Flush();
                var builder = new StringBuilder(Encoding.UTF8.GetString(stream.ToArray()));
                return builder.ToString();
            }
        }
        catch (Exception ex)
        {
            Systems.LogWriter.Error(string.Format("Exception Message : {0} Stack : {1} Prev Func Name : {2} }", ex.Message.ToString(), ex.StackTrace.ToString(), $"{new StackFrame(1, true).GetMethod().Name}"));
            throw ex;
        }
    }

    public static IEqualityComparer<string> DefaultComparer = new CaseInsensitiveStringComparer();

    class CaseInsensitiveStringComparer : IEqualityComparer<string> 
    {
        public bool Equals(string x, string y) {
            return String.Compare(x, y, true) == 0;
        }

        public int GetHashCode(string obj) {
            return obj.ToLowerInvariant().GetHashCode();
        }

#if JS
        public new bool Equals(object x, object y) {
            var xs = x as string;
            var ys = y as string;
            if (xs == null || ys == null) {
                return xs == null && ys == null;
            }
            return Equals(xs, ys);
        }

        public int GetHashCode(object obj) {
            if (obj is string) {
                return GetHashCode((string)obj);
            }
            return obj.ToStringInvariant().ToLowerInvariant().GetHashCode();
        }
#endif
    }
}

public class IniSection : IEnumerable<KeyValuePair<string, IniValue>>, IDictionary<string, IniValue> {
    private Dictionary<string, IniValue> values;

    #region Ordered
    private List<string> orderedKeys;

    public int IndexOf(string key) {
        if (!Ordered) {
            throw new InvalidOperationException("Cannot call IndexOf(string) on IniSection: section was not ordered.");
        }
        return IndexOf(key, 0, orderedKeys.Count);
    }

    public int IndexOf(string key, int index) {
        if (!Ordered) {
            throw new InvalidOperationException("Cannot call IndexOf(string, int) on IniSection: section was not ordered.");
        }
        return IndexOf(key, index, orderedKeys.Count - index);
    }

    public int IndexOf(string key, int index, int count) {
        if (!Ordered) {
            throw new InvalidOperationException("Cannot call IndexOf(string, int, int) on IniSection: section was not ordered.");
        }
        if (index < 0 || index > orderedKeys.Count) {
            throw new IndexOutOfRangeException("Index must be within the bounds." + Environment.NewLine + "Parameter name: index");
        }
        if (count < 0) {
            throw new IndexOutOfRangeException("Count cannot be less than zero." + Environment.NewLine + "Parameter name: count");
        }
        if (index + count > orderedKeys.Count) {
            throw new ArgumentException("Index and count were out of bounds for the array or count is greater than the number of elements from index to the end of the source collection.");
        }
        var end = index + count;
        for (int i = index; i < end; i++) {
            if (Comparer.Equals(orderedKeys[i], key)) {
                return i;
            }
        }
        return -1;
    }

    public int LastIndexOf(string key) {
        if (!Ordered) {
            throw new InvalidOperationException("Cannot call LastIndexOf(string) on IniSection: section was not ordered.");
        }
        return LastIndexOf(key, 0, orderedKeys.Count);
    }

    public int LastIndexOf(string key, int index) {
        if (!Ordered) {
            throw new InvalidOperationException("Cannot call LastIndexOf(string, int) on IniSection: section was not ordered.");
        }
        return LastIndexOf(key, index, orderedKeys.Count - index);
    }

    public int LastIndexOf(string key, int index, int count) {
        if (!Ordered) {
            throw new InvalidOperationException("Cannot call LastIndexOf(string, int, int) on IniSection: section was not ordered.");
        }
        if (index < 0 || index > orderedKeys.Count) {
            throw new IndexOutOfRangeException("Index must be within the bounds." + Environment.NewLine + "Parameter name: index");
        }
        if (count < 0) {
            throw new IndexOutOfRangeException("Count cannot be less than zero." + Environment.NewLine + "Parameter name: count");
        }
        if (index + count > orderedKeys.Count) {
            throw new ArgumentException("Index and count were out of bounds for the array or count is greater than the number of elements from index to the end of the source collection.");
        }
        var end = index + count;
        for (int i = end - 1; i >= index; i--) {
            if (Comparer.Equals(orderedKeys[i], key)) {
                return i;
            }
        }
        return -1;
    }

    public void Insert(int index, string key, IniValue value) {
        if (!Ordered) {
            throw new InvalidOperationException("Cannot call Insert(int, string, IniValue) on IniSection: section was not ordered.");
        }
        if (index < 0 || index > orderedKeys.Count) {
            throw new IndexOutOfRangeException("Index must be within the bounds." + Environment.NewLine + "Parameter name: index");
        }
        values.Add(key, value);
        orderedKeys.Insert(index, key);
    }

    public void InsertRange(int index, IEnumerable<KeyValuePair<string, IniValue>> collection) {
        if (!Ordered) {
            throw new InvalidOperationException("Cannot call InsertRange(int, IEnumerable<KeyValuePair<string, IniValue>>) on IniSection: section was not ordered.");
        }
        if (collection == null) {
            throw new ArgumentNullException("Value cannot be null." + Environment.NewLine + "Parameter name: collection");
        }
        if (index < 0 || index > orderedKeys.Count) {
            throw new IndexOutOfRangeException("Index must be within the bounds." + Environment.NewLine + "Parameter name: index");
        }
        foreach (var kvp in collection) {
            Insert(index, kvp.Key, kvp.Value);
            index++;
        }
    }

    public void RemoveAt(int index) {
        if (!Ordered) {
            throw new InvalidOperationException("Cannot call RemoveAt(int) on IniSection: section was not ordered.");
        }
        if (index < 0 || index > orderedKeys.Count) {
            throw new IndexOutOfRangeException("Index must be within the bounds." + Environment.NewLine + "Parameter name: index");
        }
        var key = orderedKeys[index];
        orderedKeys.RemoveAt(index);
        values.Remove(key);
    }

    public void RemoveRange(int index, int count) {
        if (!Ordered) {
            throw new InvalidOperationException("Cannot call RemoveRange(int, int) on IniSection: section was not ordered.");
        }
        if (index < 0 || index > orderedKeys.Count) {
            throw new IndexOutOfRangeException("Index must be within the bounds." + Environment.NewLine + "Parameter name: index");
        }
        if (count < 0) {
            throw new IndexOutOfRangeException("Count cannot be less than zero." + Environment.NewLine + "Parameter name: count");
        }
        if (index + count > orderedKeys.Count) {
            throw new ArgumentException("Index and count were out of bounds for the array or count is greater than the number of elements from index to the end of the source collection.");
        }
        for (int i = 0; i < count; i++) {
            RemoveAt(index);
        }
    }

    public void Reverse() {
        if (!Ordered) {
            throw new InvalidOperationException("Cannot call Reverse() on IniSection: section was not ordered.");
        }
        orderedKeys.Reverse();
    }

    public void Reverse(int index, int count) {
        if (!Ordered) {
            throw new InvalidOperationException("Cannot call Reverse(int, int) on IniSection: section was not ordered.");
        }
        if (index < 0 || index > orderedKeys.Count) {
            throw new IndexOutOfRangeException("Index must be within the bounds." + Environment.NewLine + "Parameter name: index");
        }
        if (count < 0) {
            throw new IndexOutOfRangeException("Count cannot be less than zero." + Environment.NewLine + "Parameter name: count");
        }
        if (index + count > orderedKeys.Count) {
            throw new ArgumentException("Index and count were out of bounds for the array or count is greater than the number of elements from index to the end of the source collection.");
        }
        orderedKeys.Reverse(index, count);
    }

    public ICollection<IniValue> GetOrderedValues() {
        if (!Ordered) {
            throw new InvalidOperationException("Cannot call GetOrderedValues() on IniSection: section was not ordered.");
        }
        var list = new List<IniValue>();
        for (int i = 0; i < orderedKeys.Count; i++) {
            list.Add(values[orderedKeys[i]]);
		}
        return list;
    }

    public IniValue this[int index] {
        get {
            if (!Ordered) {
                throw new InvalidOperationException("Cannot index IniSection using integer key: section was not ordered.");
            }
            if (index < 0 || index >= orderedKeys.Count) {
                throw new IndexOutOfRangeException("Index must be within the bounds." + Environment.NewLine + "Parameter name: index");
            }
            return values[orderedKeys[index]];
        }
        set {
            if (!Ordered) {
                throw new InvalidOperationException("Cannot index IniSection using integer key: section was not ordered.");
            }
            if (index < 0 || index >= orderedKeys.Count) {
                throw new IndexOutOfRangeException("Index must be within the bounds." + Environment.NewLine + "Parameter name: index");
            }
            var key = orderedKeys[index];
            values[key] = value;
        }
    }

    public bool Ordered {
        get {
            return orderedKeys != null;
        }
        set {
            if (Ordered != value) {
                orderedKeys = value ? new List<string>(values.Keys) : null;
            }
        }
    }
    #endregion

    public IniSection()
        : this(IniFile.DefaultComparer) {
    }

    public IniSection(IEqualityComparer<string> stringComparer)
    {
        try
        {
            this.values = new Dictionary<string, IniValue>(stringComparer);
        }
        catch (Exception ex)
        {
            Systems.LogWriter.Error(string.Format("Exception Message : {0} Stack : {1} Prev Func Name : {2} }", ex.Message.ToString(), ex.StackTrace.ToString(), $"{new StackFrame(1, true).GetMethod().Name}"));
            throw ex;
        }
    }

    public IniSection(Dictionary<string, IniValue> values)
        : this(values, IniFile.DefaultComparer) {
    }

    public IniSection(Dictionary<string, IniValue> values, IEqualityComparer<string> stringComparer) 
    {
        try
        {
            this.values = new Dictionary<string, IniValue>(values, stringComparer);
        }
        catch (Exception ex)
        {
            Systems.LogWriter.Error(string.Format("Exception Message : {0} Stack : {1} Prev Func Name : {2} }", ex.Message.ToString(), ex.StackTrace.ToString(), $"{new StackFrame(1, true).GetMethod().Name}"));
            throw ex;
        }
    }

    public IniSection(IniSection values)
        : this(values, IniFile.DefaultComparer) {
    }

    public IniSection(IniSection values, IEqualityComparer<string> stringComparer) 
    {
        try
        {
            this.values = new Dictionary<string, IniValue>(values.values, stringComparer);
        }
        catch (Exception ex)
        {
            Systems.LogWriter.Error(string.Format("Exception Message : {0} Stack : {1} Prev Func Name : {2} }", ex.Message.ToString(), ex.StackTrace.ToString(), $"{new StackFrame(1, true).GetMethod().Name}"));
            throw ex;
        }
    }

    public void Add(string key, IniValue value) 
    {
        try
        {
            values.Add(key, value);
            if (Ordered)
            {
                orderedKeys.Add(key);
            }
        }
        catch (Exception ex)
        {
            Systems.LogWriter.Error(string.Format("Exception Message : {0} Stack : {1} Prev Func Name : {2} }", ex.Message.ToString(), ex.StackTrace.ToString(), $"{new StackFrame(1, true).GetMethod().Name}"));
            throw ex;
        }
    }

    public bool ContainsKey(string key) 
    {
        try
        {
            return values.ContainsKey(key);
        }
        catch (Exception ex)
        {
            Systems.LogWriter.Error(string.Format("Exception Message : {0} Stack : {1} Prev Func Name : {2} }", ex.Message.ToString(), ex.StackTrace.ToString(), $"{new StackFrame(1, true).GetMethod().Name}"));
            throw ex;
        }
    }

    /// <summary>
    /// Returns this IniSection's collection of keys. If the IniSection is ordered, the keys will be returned in order.
    /// </summary>
    public ICollection<string> Keys {
        get { return Ordered ? (ICollection<string>)orderedKeys : values.Keys; }
    }

    public bool Remove(string key)
    {
        try
        {
            var ret = values.Remove(key);
            if (Ordered && ret)
            {
                for (int i = 0; i < orderedKeys.Count; i++)
                {
                    if (Comparer.Equals(orderedKeys[i], key))
                    {
                        orderedKeys.RemoveAt(i);
                        break;
                    }
                }
            }
            return ret;
        }
        catch (Exception ex)
        {
            Systems.LogWriter.Error(string.Format("Exception Message : {0} Stack : {1} Prev Func Name : {2} }", ex.Message.ToString(), ex.StackTrace.ToString(), $"{new StackFrame(1, true).GetMethod().Name}"));
            throw ex;
        }
    }

    public bool TryGetValue(string key, out IniValue value) 
    {
        try
        {
            return values.TryGetValue(key, out value);
        }
        catch (Exception ex)
        {
            Systems.LogWriter.Error(string.Format("Exception Message : {0} Stack : {1} Prev Func Name : {2} }", ex.Message.ToString(), ex.StackTrace.ToString(), $"{new StackFrame(1, true).GetMethod().Name}"));
            throw ex;
        }
    }

    /// <summary>
    /// Returns the values in this IniSection. These values are always out of order. To get ordered values from an IniSection call GetOrderedValues instead.
    /// </summary>
    public ICollection<IniValue> Values {
        get {
            return values.Values;
        }
    }

    void ICollection<KeyValuePair<string, IniValue>>.Add(KeyValuePair<string, IniValue> item)
    {
        try
        {
            ((IDictionary<string, IniValue>)values).Add(item);
            if (Ordered)
            {
                orderedKeys.Add(item.Key);
            }
        }
        catch (Exception ex)
        {
            Systems.LogWriter.Error(string.Format("Exception Message : {0} Stack : {1} Prev Func Name : {2} }", ex.Message.ToString(), ex.StackTrace.ToString(), $"{new StackFrame(1, true).GetMethod().Name}"));
            throw ex;
        }
    }

    public void Clear()
    {
        try
        {
            values.Clear();
            if (Ordered)
            {
                orderedKeys.Clear();
            }
        }
        catch (Exception ex)
        {
            Systems.LogWriter.Error(string.Format("Exception Message : {0} Stack : {1} Func Name : {2} }", ex.Message.ToString(), ex.StackTrace.ToString(), $"{this.GetType().Name}"));
        }
    }

    bool ICollection<KeyValuePair<string, IniValue>>.Contains(KeyValuePair<string, IniValue> item) 
    {
        try
        {
            return ((IDictionary<string, IniValue>)values).Contains(item);
        }
        catch (Exception ex)
        {
            Systems.LogWriter.Error(string.Format("Exception Message : {0} Stack : {1} Prev Func Name : {2} }", ex.Message.ToString(), ex.StackTrace.ToString(), $"{new StackFrame(1, true).GetMethod().Name}"));
            throw ex;
        }
    }

    void ICollection<KeyValuePair<string, IniValue>>.CopyTo(KeyValuePair<string, IniValue>[] array, int arrayIndex) 
    {
        try
        {
            ((IDictionary<string, IniValue>)values).CopyTo(array, arrayIndex);
        }
        catch (Exception ex)
        {
            Systems.LogWriter.Error(string.Format("Exception Message : {0} Stack : {1} Prev Func Name : {2} }", ex.Message.ToString(), ex.StackTrace.ToString(), $"{new StackFrame(1, true).GetMethod().Name}"));
            throw ex;
        }
    }

    public int Count 
    {
        get { try { return values.Count; }
            catch (Exception ex)
            {
                Systems.LogWriter.Error(string.Format("Exception Message : {0} Stack : {1} Prev Func Name : {2} }", ex.Message.ToString(), ex.StackTrace.ToString(), $"{new StackFrame(1, true).GetMethod().Name}"));
                throw ex;
            }
        }
    }

    bool ICollection<KeyValuePair<string, IniValue>>.IsReadOnly 
    {
        get { try { return ((IDictionary<string, IniValue>)values).IsReadOnly; }
            catch (Exception ex)
            {
                Systems.LogWriter.Error(string.Format("Exception Message : {0} Stack : {1} Prev Func Name : {2} }", ex.Message.ToString(), ex.StackTrace.ToString(), $"{new StackFrame(1, true).GetMethod().Name}"));
                throw ex;
            }
        }
    }

    bool ICollection<KeyValuePair<string, IniValue>>.Remove(KeyValuePair<string, IniValue> item) 
    {
        try
        {
            var ret = ((IDictionary<string, IniValue>)values).Remove(item);
            if (Ordered && ret)
            {
                for (int i = 0; i < orderedKeys.Count; i++)
                {
                    if (Comparer.Equals(orderedKeys[i], item.Key))
                    {
                        orderedKeys.RemoveAt(i);
                        break;
                    }
                }
            }
            return ret;
        }
        catch (Exception ex)
        {
            Systems.LogWriter.Error(string.Format("Exception Message : {0} Stack : {1} Prev Func Name : {2} }", ex.Message.ToString(), ex.StackTrace.ToString(), $"{new StackFrame(1, true).GetMethod().Name}"));
            throw ex;
        }
    }

    public IEnumerator<KeyValuePair<string, IniValue>> GetEnumerator() 
    {
        try
        {
            if (Ordered)
            {
                return GetOrderedEnumerator();
            }
            else
            {
                return values.GetEnumerator();
            }
        }
        catch (Exception ex)
        {
            Systems.LogWriter.Error(string.Format("Exception Message : {0} Stack : {1} Prev Func Name : {2} }", ex.Message.ToString(), ex.StackTrace.ToString(), $"{new StackFrame(1, true).GetMethod().Name}"));
            throw ex;
        }
    }

    private IEnumerator<KeyValuePair<string, IniValue>> GetOrderedEnumerator() 
    {
        for (int i = 0; i < orderedKeys.Count; i++) {
            yield return new KeyValuePair<string, IniValue>(orderedKeys[i], values[orderedKeys[i]]);
        }
    }

    System.Collections.IEnumerator System.Collections.IEnumerable.GetEnumerator() 
    {
        try
        {
            return GetEnumerator();
        }
        catch (Exception ex)
        {
            Systems.LogWriter.Error(string.Format("Exception Message : {0} Stack : {1} Prev Func Name : {2} }", ex.Message.ToString(), ex.StackTrace.ToString(), $"{new StackFrame(1, true).GetMethod().Name}"));
            throw ex;
        }
    }

    public IEqualityComparer<string> Comparer { get { try { return values.Comparer; }
            catch (Exception ex)
            {
                Systems.LogWriter.Error(string.Format("Exception Message : {0} Stack : {1} Prev Func Name : {2} }", ex.Message.ToString(), ex.StackTrace.ToString(), $"{new StackFrame(1, true).GetMethod().Name}"));
                throw ex;
            }
        } }

    public IniValue this[string name]
    {
        get {
            try
            {
                IniValue val;
                if (values.TryGetValue(name, out val))
                {
                    return val;
                }
                return IniValue.Default;
            }
            catch (Exception ex)
            {
                Systems.LogWriter.Error(string.Format("Exception Message : {0} Stack : {1} Prev Func Name : {2} }", ex.Message.ToString(), ex.StackTrace.ToString(), $"{new StackFrame(1, true).GetMethod().Name}"));
                throw ex;
            }
        }
        set {
            try
            {
                if (Ordered && !orderedKeys.Contains(name, Comparer))
                {
                    orderedKeys.Add(name);
                }
                values[name] = value;
            }
            catch (Exception ex)
            {
                Systems.LogWriter.Error(string.Format("Exception Message : {0} Stack : {1} Prev Func Name : {2} }", ex.Message.ToString(), ex.StackTrace.ToString(), $"{new StackFrame(1, true).GetMethod().Name}"));
                throw ex;
            }
        }
    }

    public static implicit operator IniSection(Dictionary<string, IniValue> dict) {
        return new IniSection(dict);
    }

    public static explicit operator Dictionary<string, IniValue>(IniSection section) {
        return section.values;
    }
}
