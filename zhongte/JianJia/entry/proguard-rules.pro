-renamesourcefileattribute SourceFile
-keepattributes SourceFile,LineNumberTable
-dontwarn javax.annotation.**
-keepattributes Signature, InnerClasses, EnclosingMethod, Exceptions
# 蒹葭
-dontwarn poetry.jianjia.**
-keep class poetry.jianjia.** { *; }
-keepattributes RuntimeVisibleAnnotations, RuntimeVisibleParameterAnnotations
-keepclassmembers,allowshrinking,allowobfuscation interface * {
    @poetry.jianjia.http.* <methods>;
}

# OkHttp3
-dontwarn okhttp3.logging.**
-keep class okhttp3.internal.**{*;}
-dontwarn okio.**

# gson
-keep class sun.misc.Unsafe { *; }
-keep class com.google.gson.stream.** { *; }
-keepattributes *Annotation*
-keepclassmembers class * implements java.io.Serializable {
    static final long serialVersionUID;
    private static final java.io.ObjectStreamField[] serialPersistentFields;
    private void writeObject(java.io.ObjectOutputStream);
    private void readObject(java.io.ObjectInputStream);
    java.lang.Object writeReplace();
    java.lang.Object readResolve();
}
# 在我的示例代码中，com.poetry.jianjia.bean这个包下面的类实现了Serialized接口，
# 实现了Serialized接口的类不能被混淆，请把com.poetry.jianjia.bean这个包名替换成你自己的包名
-keep class com.poetry.jianjia.bean.**{*;}