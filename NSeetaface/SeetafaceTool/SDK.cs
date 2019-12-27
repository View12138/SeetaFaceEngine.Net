using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace ViewFace
{
    /// <summary>
    /// 日志回调函数
    /// </summary>
    /// <param name="logInfo"></param>
    public delegate void LogCallBack(string logInfo);

    /// <summary>
    /// 人脸识别SDK
    /// <para>[包装C++库] 请调用 <see cref="Tool"/> 中的类</para>
    /// </summary>
    class SDK
    {
        public static string Path { get; set; }
        /// <summary>
        /// 设置日志回调函数(用于日志打印)
        /// </summary>
        /// <param name="logCallBack"></param>
        [DllImport("NSeetaface.dll", EntryPoint = "SetDisplayLog", CallingConvention = CallingConvention.Cdecl)]
        public static extern void SetDisplayLog(LogCallBack logCallBack);

        /// <summary>
        /// 设置人脸模型的目录
        /// </summary>
        /// <param name="dirPath"></param>
        [DllImport("NSeetaface.dll", EntryPoint = "SetModelDirectory", CallingConvention = CallingConvention.Cdecl)]
        public extern static void SetModelDirectory(byte[] dirPath);

        /// <summary>
        /// 初始化人脸识别
        /// </summary>
        /// <returns></returns>
        [DllImport("NSeetaface.dll", EntryPoint = "Init")]
        public extern static bool Init();

        /// <summary>
        /// 检测图片中的人脸.获取第一张人脸
        /// </summary>
        /// <param name="path"></param>
        /// <param name="faceInfo"></param>
        /// <returns></returns>
        [DllImport("NSeetaface.dll", EntryPoint = "DetectFace", CallingConvention = CallingConvention.Cdecl)]
        public extern static int DetectFace(string path, ref FaceInfo faceInfo);
        /// <summary>
        /// 检测图片中的人脸.获取第一张人脸,通过内存数据
        /// </summary>
        /// <param name="path"></param>
        /// <param name="faceInfo"></param>
        /// <returns></returns>
        [DllImport("NSeetaface.dll", EntryPoint = "DetectFaceByte", CallingConvention = CallingConvention.Cdecl)]
        public extern static int DetectFaceByte(byte[] imageByte, int width, int height, int stride, ref FaceInfo faceInfo);

        /// <summary>
        /// 检测图片中的人脸,返回多张人脸JSON数据
        /// </summary>
        /// <param name="path"></param>
        /// <param name="faceInfo"></param>
        /// <returns></returns>
        [DllImport("NSeetaface.dll", EntryPoint = "DetectFaces", CallingConvention = CallingConvention.Cdecl)]
        public extern static int DetectFaces(string path, StringBuilder json);

        /// <summary>
        /// 检测图片中的人脸,返回多张人脸JSON数据,通过内存数据
        /// </summary>
        /// <param name="path"></param>
        /// <param name="faceInfo"></param>
        /// <returns></returns>
        [DllImport("NSeetaface.dll", EntryPoint = "DetectFacesByte", CallingConvention = CallingConvention.Cdecl)]
        public extern static int DetectFacesByte(byte[] imageByte, int width, int height, int stride, StringBuilder json);

        /// <summary>
        /// 人脸对齐
        /// </summary>
        /// <param name="path"></param>
        /// <param name="json"></param>
        /// <returns></returns>
        [DllImport("NSeetaface.dll", EntryPoint = "Alignment", CallingConvention = CallingConvention.Cdecl)]
        public extern static int Alignment(string path, StringBuilder json);

        /// <summary>
        /// 人脸对齐,通过内存数据
        /// </summary>
        /// <param name="path"></param>
        /// <param name="json"></param>
        /// <returns></returns>
        [DllImport("NSeetaface.dll", EntryPoint = "AlignmentByte", CallingConvention = CallingConvention.Cdecl)]
        public extern static int AlignmentByte(byte[] imageByte, int width, int height, int stride, StringBuilder json);

        /// <summary>
        /// 提取特征
        /// </summary>
        /// <param name="picPath"></param>
        /// <param name="face"></param>
        /// <param name="pt5"></param>
        /// <returns></returns>
        [DllImport("NSeetaface.dll", EntryPoint = "ExtractFeature", CallingConvention = CallingConvention.Cdecl)]
        public extern static bool ExtractFeature(string picPath, ref AlignmentResult alignmentResult, float[] feat);

        /// <summary>
        /// 提取特征,通过内存数据
        /// </summary>
        /// <param name="width">图像宽度</param>
        /// <param name="height">图像高度</param>
        /// <param name="data">图像的字节数组</param>
        /// <param name="face"></param>
        /// <param name="pt5"></param>
        /// <returns></returns>
        [DllImport("NSeetaface.dll", EntryPoint = "ExtracFeatureByte", CallingConvention = CallingConvention.Cdecl)]
        public extern static bool ExtracFeatureByte(byte[] imageByte, int width, int height, int stride, ref AlignmentResult alignmentResult, float[] feat);

        /// <summary>
        /// 计算相似度
        /// </summary>
        /// <param name="feat1">人脸特征1</param>
        /// <param name="feat2">人脸特征2</param>
        /// <returns></returns>
        [DllImport("NSeetaface.dll", EntryPoint = "CalcSimilarity", CallingConvention = CallingConvention.Cdecl)]
        public extern static double CalcSimilarity(float[] feat1, float[] feat2);
    }
}
