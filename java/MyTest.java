public class MyTest
{
    private static int magic_counter = 777;

	// c����java����̬�����޲���
    public static void mymain1()
    {
        System.out.println("Hello, World in java from mymain");
        System.out.println(magic_counter);
    }

    // c����java����̬�������������з���ֵ
    public static int mymain2(int n)
    {
        for (int i = 0; i < n; i++)
        {
            System.out.print (i);
            System.out.println("Hello, World !");
        }
        return n * 2;
    }

    public static void main (String[] args)
    {
        System.out.println("Hello, World in java");
        int i;
        for (i = 0; i < args.length; i++)
            System.out.println(args[i]);
    }

    //<=========================== 
	// ���캯��
    private int uid = 123;					// ˽�г�Ա
    public MyTest()
    {
        uid = magic_counter++ * 2;
    }

	// java�ص�c���в������޲������ݲ����Ƿ���ֵ
    public void showId()
    {
        System.out.println(uid);
        doTest1();
		uid++;
        int result = doTest2(123);
		System.out.println("result from c language: " + result);
    }

    public native void doTest1();			// c����JNIʵ��
    public native int doTest2(int n);		// c����JNIʵ��
}

