using System;
using System.Drawing;
using System.Drawing.Imaging;
using System.IO;
using System.Runtime.InteropServices;
using System.Text;
using Newtonsoft.Json;

namespace ViewFace
{
    /// <summary>
    /// <see langword="SeetaFace 1.0"/> 人脸识别库 <see langword=".Net"/> 工具类。
    /// <para>使用方法：</para>
    /// <para>0. 搭建环境。</para>
    /// <para>1. 调用 <see cref="SetModelDirectory(string)"/> 设置 <see langword="seeta_*_v1.?.bin"/> 模块路径。</para>
    /// <para>2. 调用 <see cref="Init"/> 初始化人脸识别库。</para>
    /// </summary>
    public class Tool
    {
        // 私有方法
        /// <summary>
        /// <see cref="Bitmap"/> 转 byte[]
        /// <para>释放此 bitmap 使用的所有资源</para>
        /// </summary>
        /// <param name="bitmap">待转换图像</param>
        /// <param name="width">图像宽度</param>
        /// <param name="height">图像高度</param>
        /// <param name="stride">图像扫描宽度</param>
        /// <returns></returns>
        private static byte[] BitmapToByteBuffer(Bitmap bitmap, out int width, out int height, out int stride)
        {
            Rectangle rect = new System.Drawing.Rectangle(0, 0, bitmap.Width, bitmap.Height);
            BitmapData bmpData = bitmap.LockBits(rect, ImageLockMode.ReadWrite, bitmap.PixelFormat);
            IntPtr ptr = bmpData.Scan0; // 获取数据起始位置
            int bytesLength = Math.Abs(bmpData.Stride) * bitmap.Height; // 获取整个 Bitmap 在内存空间中的长度
            width = bitmap.Width;
            height = bitmap.Height;
            stride = bmpData.Stride;
            byte[] buffer = new byte[bytesLength];
            // Copy the RGB values into the array.
            Marshal.Copy(ptr, buffer, 0, bytesLength);
            bitmap.UnlockBits(bmpData);
            bitmap.Dispose();
            return buffer;
        }

        //公共方法
        /// <summary>
        /// 设置人脸模型的目录
        /// </summary>
        /// <param name="modelPath"></param>
        public static void SetModelDirectory(string modelPath) => SDK.SetModelDirectory(Encoding.Default.GetBytes(modelPath));
        /// <summary>
        /// 初始化人脸识别
        /// </summary>
        /// <returns></returns>
        public static bool Init() => SDK.Init();
        /// <summary>
        /// 设置日志回调函数
        /// </summary>
        /// <param name="logCallBack"></param>
        public static void SetDisplayLog(LogCallBack logCallBack) => SDK.SetDisplayLog(logCallBack);
        /// <summary>
        /// 检测图片中的人脸，返回识别到的人脸的数量。
        /// </summary>
        /// <param name="path">图片的路径</param>
        /// <param name="faceInfo">识别到的第一张人脸</param>
        /// <returns></returns>
        public static int DetectFace(string path, ref FaceInfo faceInfo)
        {
            return SDK.DetectFace(path, ref faceInfo);
        }
        /// <summary>
        /// 检测图片中的人脸，返回识别到的人脸的数量。
        /// </summary>
        /// <param name="bitmap">图片</param>
        /// <param name="faceInfo">识别到的第一张人脸</param>
        /// <returns></returns>
        public static int DetectFace(Bitmap bitmap, ref FaceInfo faceInfo)
        {
            byte[] imageByte = BitmapToByteBuffer(bitmap, out int width, out int height, out int stride);
            return SDK.DetectFaceByte(imageByte, width, height, stride, ref faceInfo);
        }
        /// <summary>
        /// 检测图片中的人脸，返回识别到的人脸数量。
        /// </summary>
        /// <param name="path">图片的路径</param>
        /// <param name="faceInfos">识别到的所有人脸的信息</param>
        /// <returns></returns>
        public static int DetectFaces(string path, ref FaceInfo[] faceInfos)
        {
            StringBuilder json = new StringBuilder(500);
            int faceCount = SDK.DetectFaces(path, json);
            JsonSerializer serializer = new JsonSerializer();
            if (faceCount > 0)
            {
                faceInfos = serializer.Deserialize<FaceInfo[]>(new JsonTextReader(new StringReader(json.ToString())));
                return faceCount;
            }
            else
            {
                faceInfos = new FaceInfo[0];
                return faceCount;
            }
        }
        /// <summary>
        /// 检测图片中的人脸，返回识别到的人脸数量。
        /// </summary>
        /// <param name="bitmap">图片</param>
        /// <param name="faceInfos">识别到的所有人脸的信息</param>
        /// <returns></returns>
        public static int DetectFaces(Bitmap bitmap, ref FaceInfo[] faceInfos)
        {
            byte[] imageByte = BitmapToByteBuffer(bitmap, out int width, out int height, out int stride);
            StringBuilder json = new StringBuilder(500);
            int faceCount = SDK.DetectFacesByte(imageByte, width, height, stride, json);
            JsonSerializer serializer = new JsonSerializer();
            if (faceCount > 0)
            {
                faceInfos = serializer.Deserialize<FaceInfo[]>(new JsonTextReader(new StringReader(json.ToString())));
                return faceCount;
            }
            else
            {
                faceInfos = new FaceInfo[0];
                return faceCount;
            }
        }
        /// <summary>
        /// 人脸对齐，返回识别到的人脸数量。
        /// </summary>
        /// <param name="path">图片的路径</param>
        /// <param name="faces">人脸对齐的结果</param>
        /// <returns></returns>
        public static int Alignment(string path, ref AlignmentResult[] faces)
        {
            StringBuilder json = new StringBuilder(1500);
            int faceCount = SDK.Alignment(path, json);
            if (faceCount > 0)
            {
                JsonSerializer serializer = new JsonSerializer();
                faces = serializer.Deserialize<AlignmentResult[]>(new JsonTextReader(new StringReader(json.ToString())));
                return faceCount;
            }
            else
            {
                faces = new AlignmentResult[0];
                return faceCount;
            }
        }
        /// <summary>
        /// 人脸对齐，返回识别到的人脸数量。
        /// </summary>
        /// <param name="bitmap">图片</param>
        /// <param name="faces">人脸对齐的结果</param>
        /// <returns></returns>
        public static int Alignment(Bitmap bitmap, ref AlignmentResult[] faces)
        {
            byte[] imageByte = BitmapToByteBuffer(bitmap, out int width, out int height, out int stride);
            StringBuilder json = new StringBuilder(1500);
            int faceCount = SDK.AlignmentByte(imageByte, width, height, stride, json);
            if (faceCount > 0)
            {
                JsonSerializer serializer = new JsonSerializer();
                faces = serializer.Deserialize<AlignmentResult[]>(new JsonTextReader(new StringReader(json.ToString())));
                return faceCount;
            }
            else
            {
                faces = new AlignmentResult[0];
                return faceCount;
            }
        }
        /// <summary>
        /// 提取人脸特征码。
        /// </summary>
        /// <param name="path">图片路径</param>
        /// <param name="alignmentResult">要提取特征码的人脸的对齐结果</param>
        /// <param name="feat">特征码</param>
        /// <returns></returns>
        public static bool ExtractFeature(string path, ref AlignmentResult alignmentResult, float[] feat)
        {
            return SDK.ExtractFeature(path, ref alignmentResult, feat);
        }
        /// <summary>
        /// 提取人脸特征码。
        /// </summary>
        /// <param name="bitmap">图片</param>
        /// <param name="alignmentResult">要提取特征码的人脸的对齐结果</param>
        /// <param name="feat">特征码</param>
        /// <returns></returns>
        public static bool ExtractFeature(Bitmap bitmap, ref AlignmentResult alignmentResult, float[] feat)
        {
            byte[] imageByte = BitmapToByteBuffer(bitmap, out int width, out int height, out int stride);
            return SDK.ExtracFeatureByte(imageByte, width, height, stride, ref alignmentResult, feat);
        }
        /// <summary>
        /// 计算人脸特征码相似度。
        /// </summary>
        /// <param name="feat1">人脸特征码1</param>
        /// <param name="feat2">人脸特征码2</param>
        /// <returns></returns>
        public static double CalcSimilarity(float[] feat1, float[] feat2) => SDK.CalcSimilarity(feat1, feat2);
    }
}
