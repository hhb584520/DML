/*
 * Copyright 2020-2021 Intel Corporation.
 *
 * This software and the related documents are Intel copyrighted materials,
 * and your use of them is governed by the express license under which they
 * were provided to you ("License"). Unless the License provides otherwise,
 * you may not use, modify, copy, publish, distribute, disclose or transmit
 * this software or the related documents without Intel's prior written
 * permission.
 *
 * This software and the related documents are provided as is, with no
 * express or implied warranties, other than those that are expressly
 * stated in the License.
 *
 */

/**
 * @brief Contains a software implementation of @ref DML_OP_COPY_CRC operation
 * @date 3/12/2020
 *
 */

OWN_FUN_INLINE(dml_status_t, sw_copy_with_crc, (dml_job_t *const dml_job_ptr))
{
    DML_BAD_ARGUMENT_NULL_POINTER(dml_job_ptr->source_first_ptr)
    DML_BAD_ARGUMENT_NULL_POINTER(dml_job_ptr->destination_first_ptr)
    DML_BAD_ARGUMENT_NULL_POINTER(dml_job_ptr->crc_checksum_ptr)
    DML_BAD_ARGUMENT_BUFFERS_OVERLAPPING(dml_job_ptr->source_first_ptr, dml_job_ptr->source_length,
                                         dml_job_ptr->destination_first_ptr, dml_job_ptr->destination_length)

    // Variables
    dml_status_t status;

    uint8_t *const source_ptr      = dml_job_ptr->source_first_ptr;
    uint8_t *const destination_ptr = dml_job_ptr->destination_first_ptr;
    const uint32_t byte_size       = dml_job_ptr->source_length;

    // Copy
    status = dmlc_copy_8u(source_ptr, destination_ptr, byte_size);

    DML_RETURN_IN_CASE_OF_ERROR(status)

    // Crc calculation
    return idml_sw_crc(dml_job_ptr);
}