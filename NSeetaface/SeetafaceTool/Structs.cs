using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Text;

namespace ViewFace
{
    /// <summary>
    /// 位置矩形
    /// </summary>
    [StructLayout(LayoutKind.Sequential)]
    public struct Rect
    {
        /// <summary>
        /// x坐标
        /// </summary>
        public int x;
        /// <summary>
        /// y坐标
        /// </summary>
        public int y;
        /// <summary>
        /// 宽度
        /// </summary>
        public int width;
        /// <summary>
        /// 高度
        /// </summary>
        public int height;
    }

    /// <summary>
    /// 人脸信息
    /// </summary>
    [StructLayout(LayoutKind.Sequential)]
    public struct FaceInfo
    {
        /// <summary>
        /// 人脸所在位置
        /// </summary>
        public Rect bbox;
        public double roll;
        public double pitch;
        public double yaw;
        public double score; //< Larger score should mean higher confidence.
    }

    /// <summary>
    /// 
    /// </summary>
    [StructLayout(LayoutKind.Sequential)]
    public struct FacialLandmark
    {
        public double x;
        public double y;
    }

    /// <summary>
    /// 人脸对齐结果
    /// </summary>
    [StructLayout(LayoutKind.Sequential, Pack = 1)]
    public struct AlignmentResult
    {
        /// <summary>
        /// 人脸信息
        /// </summary>
        public FaceInfo face;
        [MarshalAs(UnmanagedType.ByValArray, ArraySubType = UnmanagedType.Struct, SizeConst = 5)]
        public FacialLandmark[] landmark;
    }
}
