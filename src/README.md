# nbtp3具体技术细节

## 最后的图片

每块图片都是3M.但是有的图片不能是3M整，因为文件的最后一块了。
文件的最后一块需要被直接编码，不能走常规流程。