#define __CLC_DEFINE_ASYNC_STRIDED_COPY(DST_SPACE, SRC_SPACE, TYPE) \
  event_t \
  __bugle_async_work_group_strided_copy_##SRC_SPACE##_to_##DST_SPACE##_##TYPE( \
      DST_SPACE TYPE *dst, const SRC_SPACE TYPE *src, \
      size_t num_elements, size_t stride, event_t event); \
  \
  _CLC_INLINE _CLC_OVERLOAD event_t \
  __bugle_async_work_group_strided_copy( \
      DST_SPACE TYPE *dst, const SRC_SPACE TYPE *src, \
      size_t num_elements, size_t stride, event_t event) { \
    return __bugle_async_work_group_strided_copy_##SRC_SPACE##_to_##DST_SPACE##_##TYPE(dst, src, num_elements, stride, event); \
  }

#define __CLC_DECLARE_ASYNC_STRIDED_COPY(DST_SPACE, SRC_SPACE, TYPE) \
  __CLC_DEFINE_ASYNC_STRIDED_COPY(DST_SPACE, SRC_SPACE, TYPE)

__CLC_DECLARE_ASYNC_STRIDED_COPY(__CLC_DST_ADDR_SPACE, __CLC_SRC_ADDR_SPACE, __CLC_GENTYPE)

#undef __CLC_DEFINE_ASYNC_STRIDED_COPY
#undef __CLC_DECLARE_ASYNC_STRIDED_COPY

