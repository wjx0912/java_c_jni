#include <tchar.h>
#include <windows.h>
#include <memory>
#include <iostream>
#include "jni.h"

#define __cpp_defer_stringify_(a,b,c,d)		a##b##c##d
#define __cpp_defer_stringify(a,b,c,d)		__cpp_defer_stringify_(a,b,c,d)

#define defer(type, ptr, func)										\
std::shared_ptr<type> __cpp_defer_stringify(__defer,type,ptr,__LINE__)(ptr, [](type *p) {		\
    if (p) {func(p);}    });

#define defer_no_check(type, ptr, func)									\
std::shared_ptr<type> __cpp_defer_stringify(__defer,type,ptr,__LINE__)(ptr, [](type *p) {		\
    func(p);    });

#define defer_raw(type, ptr,block)									\
std::shared_ptr<type> __cpp_defer_stringify(__defer,type,ptr,__LINE__)(ptr, [&](type *p) {		\
    block;    });

using namespace std;


void doTestCPP1(JNIEnv* e, jobject o) {
	std::cout << "C++callback activated1" << std::endl;
	jfieldID f_uid = e->GetFieldID(e->GetObjectClass(o), "uid", "I");
	if (f_uid)
		std::cout << "UID data member: " << e->GetIntField(o, f_uid) << std::endl;
	else std::cout << "UID not found" << std::endl;
}

int doTestCPP2(JNIEnv* e, jobject o, int param1) {
	std::cout << "C++callback activated2, param: " << param1 << std::endl;
	jfieldID f_uid = e->GetFieldID(e->GetObjectClass(o), "uid", "I");
	if (f_uid)
		std::cout << "UID data member: " << e->GetIntField(o, f_uid) << std::endl;
	else std::cout << "UID not found" << std::endl;
	return 456;
}


int main()
{
	TCHAR jvm_path[MAX_PATH] = L"C:\\Program Files (x86)\\Java\\jdk1.8.0_251\\jre\\bin\\client\\jvm.dll";
	HMODULE handle = LoadLibrary(jvm_path);
	if (!handle)
		return -1;
	defer_raw(HMODULE, NULL, FreeLibrary(handle));

	typedef jint (JNICALL * JNI_CreateJavaVMT)(JavaVM * *pvm, void** penv, void* args);
	JNI_CreateJavaVMT pJNI_CreateJavaVM = (JNI_CreateJavaVMT)GetProcAddress(handle, "JNI_CreateJavaVM");
	if (!pJNI_CreateJavaVM)
		return -2;

	JavaVM	*jvm;				// ���룺JVM ָ��(Java Virtual Machine)
	JNIEnv	*env;				// ���ݣ�jar�ȳ��򻷾�

	JavaVMInitArgs vm_args;
	JavaVMOption options[2];
	options[0].optionString = (char *)"-Djava.class.path=.;D:/MyTest.jar";	//����ָ����Ҫʹ�õĵ�����Jar��  
	options[1].optionString = (char*)"-verbose:NONE";			//���ڸ�������ʱ����Ϣ  
	vm_args.version = JNI_VERSION_1_6;	// ���jvm�汾
	vm_args.options = options;
	vm_args.nOptions = sizeof(options) / sizeof(JavaVMOption);
	vm_args.ignoreUnrecognized = JNI_FALSE;	// ���optionString����jvm������ᴴ��ʧ�ܣ�����false
	jint res = pJNI_CreateJavaVM(&jvm, (void**)&env, &vm_args);
	if (res != JNI_OK) {
		printf("create jvm failed, please check vm_args.\n");
		return -3;
	}
	printf("create jvm success.\n");
	jint ver = env->GetVersion();
	printf("jvm version: %d.%d\n", ((ver >> 16) & 0x0f), (ver & 0x0f));
	jclass myclass = env->FindClass("MyTest");  // try to find the class 
	if (myclass != nullptr) {
		// ����1����̬�����޲���
		jmethodID mid1 = env->GetStaticMethodID(myclass, "mymain1", "()V");
		if (mid1 != nullptr)
			env->CallStaticVoidMethod(myclass, mid1);
		// ����2����̬������һ��int����
		jmethodID mid2 = env->GetStaticMethodID(myclass, "mymain2", "(I)I");
		if (mid2 != nullptr)
			env->CallStaticVoidMethod(myclass, mid2, (jint)5);
		// ����3����̬������һ��string�������
		jmethodID mid3 = env->GetStaticMethodID(myclass, "main", "([Ljava/lang/String;)V");
		if (mid2 != nullptr) {
			jobjectArray arr = env->NewObjectArray(5	// �����С��5
				, env->FindClass("java/lang/String")	// ����������String
				, env->NewStringUTF("my_str"));		// ÿ��������ĳ�ʼֵ��"my_str"
			env->SetObjectArrayElement(arr, 1, env->NewStringUTF("MYOWNSTRING"));	// �ı������Ա1
			env->CallStaticVoidMethod(myclass, mid3, arr);	// ����java�ӿ�
			env->DeleteLocalRef(arr);			// �ͷ���Դ
		}
	}

	// �Զ��巽��ע�������������
	JNINativeMethod methods[]{
		{   (char *)"doTest1", (char*)"()V",  &doTestCPP1 }
		, { (char *)"doTest2", (char*)"(I)I", &doTestCPP2 }
	};
	int func_count = sizeof(methods) / sizeof(JNINativeMethod);
	res = env->RegisterNatives(myclass, methods, func_count);
	if (JNI_OK == res) {
		// ����4���Ǿ�̬������c���½�java����Ȼ����ã����Լ��������������ص�c����
		// �ص���1������������2��������
		// c�еĻص����飬�������أ������ʱ�������߳�����װ

		// (1)���ҹ��캯�����޲���
		jmethodID ctor = env->GetMethodID(myclass, "<init>", "()V");
		if (ctor != nullptr) {
			jobject myobject = env->NewObject(myclass, ctor);
			if (myobject) {
				jmethodID show = env->GetMethodID(myclass, "showId", "()V");
				if (show != nullptr) {
					env->CallVoidMethod(myobject, show);
				}
			}
		}
	} else {
		printf("fetal error: register function to jvm failed.\n");
	}
	jvm->DestroyJavaVM();
}

