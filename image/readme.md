使用C实现matlab的dbscan函数，并且对比测试

参考：

matlab文档： https://ww2.mathworks.cn/help/releases/R2020b/stats/dbscan.html?searchHighlight=dbscan&s_tid=doc_srchtitle#mw_89138510-569d-4439-9beb-75e575e0c814

论文：DBSCAN Revisited, Revisited Why and How You Should (Still) Use DBSCAN

B站视频：https://www.bilibili.com/video/BV114411M7ci/?from=search&seid=18189888639918894760&spm_id_from=333.337.0.0&vd_source=756f40649b668da6bb6e4276020ad242

#### 例子1

pc.mat, pc.bin的点云数据可视化结果如下所示

![](.\matlab_gscatter_1.jpg)

matlab 的dbscan函数聚类的结果有10个簇，他们的点数和中心坐标如下：

![](.\matlab_result1.jpg)

c语言的结果为：

![](.\c_result1.jpg)

可以看出是几乎一样的，但是有一个错误，c语言的结果少了一个簇！！说明可能还是有些问题的

#### 例子2





#### Tips

1. 是