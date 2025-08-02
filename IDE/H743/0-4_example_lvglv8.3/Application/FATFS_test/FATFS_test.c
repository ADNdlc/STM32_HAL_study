//
// Created by 12114 on 25-7-30.
//

#include "FATFS_test.h"

#include <stdio.h>
#include "ff.h"
#include "fatfs.h"

void fatfs_test(void) {
    printf(" ====================== fatfs_test ======================\r\n");
    /* =================================== 驱动器挂载 ========================================= */
    //文件系统句柄名称, 驱动器号, 1:立即挂载 0:操作时挂载
    FRESULT res = f_mount(&SDFatFS, "0:", 1);//挂载卷
    if (res == FR_OK) {             //挂载成功
        printf(" File system initialized success !!!\r\n\r\n");
    }
    else if (res == FR_NO_FILESYSTEM) {  //没有文件系统
        printf(" No file system\r\n");
        printf(" Create file system...\r\n");
        // 驱动器号, 系统类型, 簇大小(字节)
        BYTE workbuffer[4 * BLOCKSIZE];	//格式化缓冲区 = 4*SD块大小
        DWORD cluster_size = 0;		// 0:自动确认簇大小
        // 驱动器号, 系统类型, 簇大小, 初始化缓冲空间, 缓冲空间大小
        res = f_mkfs("0:", FM_EXFAT, cluster_size, workbuffer, (4*BLOCKSIZE));//创建文件系统
        if (res == FR_OK) {
            printf(" File system Create success !!!\r\n");
        }
        else {
            printf(" File system Create fail !\r\n");
            printf(" Err code is %d\r\n",res);
            return;
        }
    }
    else {
        printf(" File system initialize failed !\r\n");
        printf(" Err code is %d\r\n",res);
    }

    HAL_Delay(200);
    /* =================================== 获取系统信息 ========================================= */
    FATFS *fs;  //  返回的文件系统指针
    DWORD free_clust;//剩余簇的数量
    // 驱动器号, 返回值， 返回指向文件系统的指针
    f_getfree("0:", &free_clust, &fs);//获取空闲簇数量,最后返回一个指向此驱动器的文件系统指针
    if(res != FR_OK){
		printf(" f_getfree() error !\r\n" );
		return;
    }
    //获取成功
    printf(" *** FAT disk info ***\r\n");
    DWORD total_sector = (fs->n_fatent - 2)*fs->csize;	//计算总扇区数量,(簇的数量 = n_fatent-2) * 簇大小。 n_fatent:文件分配表中条目的总数量
    DWORD free_sector = free_clust * fs->csize;			//计算空闲扇区数量
#if _MAX_SS == _MIN_SS
    DWORD total_space = (total_sector >> 11);	//计算总空间(MB)
	DWORD free_space = (free_sector >> 11);		//计算剩余空间(MB)
#endif
#if _MAX_SS != _MIN_SS
	DWORD free_space =(free_sector * fs->ssize) >> 10;
	DWORD total_space =(total_sector *fs->ssize)>> 10;
#endif
    //打印系统信息
    printf(" FAT type = %d\r\n",fs->fs_type);
    printf("  FS_FAT12 = 1\r\n");
    printf("  FS_FAT16 = 2\r\n");
    printf("  FS_FAT32 = 3\r\n");
    printf("  FS_exFAT = 4\r\n\r\n");
#if _MAX_SS == _MIN_SS
    printf(" Sector size(bytes)= %d\r\n",_MIN_SS);		//一个扇区大小,
#endif
#if _MAX_SS != _MIN_SS
    printf(" Sector size(bytes)= %d\r\n",fs->ssize);		//一个扇区大小,
#endif
    printf(" Cluster size(sectors)= %u\r\n",fs->csize); //一个簇大小,

    printf(" Total Sector count = %lu\r\n",total_sector);	//扇区数量,
    printf(" Total Cluster count = %lu\r\n",fs->n_fatent-2);	//簇数量,

    printf(" Total space = %lu(MB)\r\n",total_space);		//总空间(MB),

    printf(" free Sector count = %lu\r\n",free_sector);	//剩余扇区数
    printf(" free Cluster count = %lu\r\n",free_clust); 	//剩余簇数
    printf(" free space = %lu(MB)\r\n",free_space);		//剩余空间(MB)

    /* =================================== 文件读写测试 ========================================= */
    FIL file;			//文件对象
    FRESULT res;		//操作结果
    //文本文件写入
       //文件对象, 文件名称,  		操作模式(打开文件不存在则创建并把读写指针定位到尾端,可写)
    res = f_open(&file, "readme.txt",FA_OPEN_APPEND|FA_WRITE);//新建和打开文件用的是同一个函数,只是操作模式不同
    if(res == FR_OK){
    	f_puts("Line: hello\n", &file);
    	printf("write file OK :%S", "readme.txt");
    }else{
    	printf("write file ERR!!");
    }
    f_close(&file);//关闭文件,写入存储介质

    //二进制数据写入binary
    uint8_t temp_buf[40];



    /* =================================== 路径操作测试 ========================================= */
    DIR 	dir;		//返回的目录对象
    FILINFO dir_info;	//文件信息

    res = f_opendir(&dir,"0:");
    if (res != FR_OK){
    	f_closedir(&dir);
    	printf("dir not exist");
    }
    else {
    	printf("\r\n==============================\r\nAll DIR and FILE :");
    	while(1){
    		res = f_readdir(&dir, &dir_info);			//按顺序读取目录项
    		if(res != FR_OK || dir_info.fname[0]== 0){	//读取失败或为空

    		}
    		if(dir_info.fattrib & AM_DIR){	//判断"目录条目的文件属性"是否为路径
    			printf("DIR &s\r\n", dir_info.fname);
    		}else{							//不是路径说明是文件
    			printf("FILE %s\r\n", dir_info.fname);
    		}
    	}
    	printf("\r\n==============================");
    	f_closedir(&dir);//操作完要关闭路径对象
    }





    printf(" ==================== fatfs_test cplt ====================\r\n");
}
