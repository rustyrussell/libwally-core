#ifndef LIBWALLY_CORE_TRANSACTION_H
#define LIBWALLY_CORE_TRANSACTION_H

#include "wally_core.h"

#ifdef __cplusplus
extern "C" {
#endif

#define WALLY_TX_SEQUENCE_FINAL 0xffffffff
#define WALLY_TX_VERSION_2 2
#define WALLY_TX_MAX_VERSION 2

#define WALLY_TXHASH_LEN 32 /** Size of a transaction hash in bytes */

#define WALLY_TX_FLAG_USE_WITNESS 0x1 /* Encode witness data if present */

#define WALLY_TX_DUMMY_NULL 0x1 /* An empty witness item */
#define WALLY_TX_DUMMY_SIG  0x2 /* A dummy signature */

/** Sighash flags for transaction signing */
#define WALLY_SIGHASH_ALL          0x01
#define WALLY_SIGHASH_NONE         0x02
#define WALLY_SIGHASH_SINGLE       0x03
#define WALLY_SIGHASH_FORKID       0x40
#define WALLY_SIGHASH_ANYONECANPAY 0x80

#ifdef SWIG
struct wally_tx_input;
struct wally_tx_output;
struct wally_tx;
#else

/** A transaction witness item */
struct wally_tx_witness_item {
    unsigned char *witness;
    size_t witness_len;
};

/** A transaction witness stack */
struct wally_tx_witness_stack {
    struct wally_tx_witness_item *items;
    size_t num_items;
    size_t items_allocation_len;
};

/** A transaction input */
struct wally_tx_input {
    unsigned char txhash[WALLY_TXHASH_LEN];
    uint32_t index;
    uint32_t sequence;
    unsigned char *script;
    size_t script_len;
    struct wally_tx_witness_stack *witness;
};

/** A transaction output */
struct wally_tx_output {
    uint64_t satoshi;
    unsigned char *script;
    size_t script_len;
};

/** A parsed bitcoin transaction */
struct wally_tx {
    uint32_t version;
    uint32_t locktime;
    struct wally_tx_input *inputs;
    size_t num_inputs;
    size_t inputs_allocation_len;
    struct wally_tx_output *outputs;
    size_t num_outputs;
    size_t outputs_allocation_len;
};
#endif /* SWIG */

/**
 * Allocate and initialize a new witness stack.
 *
 * :param allocation_len: The number of items to pre-allocate space for.
 * :param output: Destination for the resulting witness stack.
 */
WALLY_CORE_API int wally_tx_witness_stack_init_alloc(
    size_t allocation_len,
    struct wally_tx_witness_stack **output);

/**
 * Add a witness to a witness stack.
 *
 * :param stack: The witness stack to add to.
 * :param witness: The witness data to add to the stack.
 * :param witness_len: Length of ``witness`` in bytes.
 */
WALLY_CORE_API int wally_tx_witness_stack_add(
    struct wally_tx_witness_stack *stack,
    const unsigned char *witness,
    size_t witness_len);

/**
 * Add a dummy witness item to a witness stack.
 *
 * :param stack: The witness stack to add to.
 * :param flags: WALLY_TX_DUMMY_ Flags indicating the type of dummy to add.
 */
WALLY_CORE_API int wally_tx_witness_stack_add_dummy(
    struct wally_tx_witness_stack *stack,
    uint32_t flags);

/**
 * Set a witness item to a witness stack.
 *
 * :param stack: The witness stack to add to.
 * :param index: Index of the item to set. The stack will grow if needed to this many items.
 * :param witness: The witness data to add to the stack.
 * :param witness_len: Length of ``witness`` in bytes.
 */
WALLY_CORE_API int wally_tx_witness_stack_set(
    struct wally_tx_witness_stack *stack,
    size_t index,
    const unsigned char *witness,
    size_t witness_len);

/**
 * Set a dummy witness item to a witness stack.
 *
 * :param stack: The witness stack to add to.
 * :param index: Index of the item to set. The stack will grow if needed to this many items.
 * :param flags: WALLY_TX_DUMMY_ Flags indicating the type of dummy to set.
 */
WALLY_CORE_API int wally_tx_witness_stack_set_dummy(
    struct wally_tx_witness_stack *stack,
    size_t index,
    uint32_t flags);

#ifndef SWIG_PYTHON
/**
 * Free a transaction witness stack allocated by `wally_tx_witness_stack_init_alloc`.
 *
 * :param stack: The transaction witness stack to free.
 */
WALLY_CORE_API int wally_tx_witness_stack_free(
    struct wally_tx_witness_stack *stack);
#endif /* SWIG_PYTHON */

/**
 * Allocate and initialize a new transaction input.
 *
 * :param txhash: The transaction hash of the transaction this input comes from.
 * :param txhash_len: Size of ``txhash`` in bytes. Must be ``WALLY_TXHASH_LEN``.
 * :param index: The zero-based index of the transaction output in ``txhash`` that
 *|     this input comes from.
 * :param sequence: The sequence number for the input.
 * :param script: The scriptSig for the input.
 * :param script_len: Size of ``script`` in bytes.
 * :param witness: The witness stack for the input, or NULL if no witness is present.
 * :param output: Destination for the resulting transaction input.
 */
WALLY_CORE_API int wally_tx_input_init_alloc(
    const unsigned char *txhash,
    size_t txhash_len,
    uint32_t index,
    uint32_t sequence,
    const unsigned char *script,
    size_t script_len,
    const struct wally_tx_witness_stack *witness,
    struct wally_tx_input **output);

#ifndef SWIG_PYTHON
/**
 * Free a transaction input allocated by `wally_tx_input_init_alloc`.
 *
 * :param input: The transaction input to free.
 */
WALLY_CORE_API int wally_tx_input_free(struct wally_tx_input *input);
#endif /* SWIG_PYTHON */

/**
 * Allocate and initialize a new transaction output.
 *
 * :param satoshi The amount of the output in satoshi.
 * :param script: The scriptPubkey for the output.
 * :param script_len: Size of ``script`` in bytes.
 * :param output: Destination for the resulting transaction output.
 */
WALLY_CORE_API int wally_tx_output_init_alloc(
    uint64_t satoshi,
    const unsigned char *script,
    size_t script_len,
    struct wally_tx_output **output);

#ifndef SWIG_PYTHON
/**
 * Free a transaction output allocated by `wally_tx_output_init_alloc`.
 *
 * :param output: The transaction output to free.
 */
WALLY_CORE_API int wally_tx_output_free(struct wally_tx_output *output);
#endif /* SWIG_PYTHON */

/**
 * Allocate and initialize a new transaction.
 *
 * :param version: The version of the transaction. Currently must be ``WALLY_TX_VERSION_2``.
 * :param locktime: The locktime of the transaction.
 * :param inputs_allocation_len: The number of inputs to pre-allocate space for.
 * :param outputs_allocation_len: The number of outputs to pre-allocate space for.
 * :param output: Destination for the resulting transaction output.
 */
WALLY_CORE_API int wally_tx_init_alloc(
    uint32_t version,
    uint32_t locktime,
    size_t inputs_allocation_len,
    size_t outputs_allocation_len,
    struct wally_tx **output);

/**
 * Add a transaction input to a transaction.
 *
 * :param tx: The transaction to add the input to.
 * :param input: The transaction input to add to ``tx``.
 */
WALLY_CORE_API int wally_tx_add_input(
    struct wally_tx *tx,
    const struct wally_tx_input *input);

/**
 * Add a transaction input to a transaction.
 *
 * :param tx: The transaction to add the input to.
 * :param txhash: The transaction hash of the transaction this input comes from.
 * :param txhash_len: Size of ``txhash`` in bytes. Must be ``WALLY_TXHASH_LEN``.
 * :param index: The zero-based index of the transaction output in ``txhash`` that
 *|     this input comes from.
 * :param sequence: The sequence number for the input.
 * :param script: The scriptSig for the input.
 * :param script_len: Size of ``script`` in bytes.
 * :param witness: The witness stack for the input, or NULL if no witness is present.
 * :param flags: Flags controlling script creation. Must be 0.
 */
WALLY_CORE_API int wally_tx_add_raw_input(
    struct wally_tx *tx,
    const unsigned char *txhash,
    size_t txhash_len,
    uint32_t index,
    uint32_t sequence,
    const unsigned char *script,
    size_t script_len,
    const struct wally_tx_witness_stack *witness,
    uint32_t flags);

/**
 * Remove a transaction input from a transaction.
 *
 * :param tx: The transaction to remove the input from.
 * :param index: The zero-based index of the input to remove.
 */
WALLY_CORE_API int wally_tx_remove_input(
    struct wally_tx *tx,
    size_t index);

/**
 * Set the scriptsig for an input in a transaction.
 *
 * :param tx: The transaction to operate on.
 * :param index: The zero-based index of the input to set the script on.
 * :param script: The scriptSig for the input.
 * :param script_len: Size of ``script`` in bytes.
 */
WALLY_CORE_API int wally_tx_set_input_script(
    const struct wally_tx *tx,
    size_t index,
    const unsigned char *script,
    size_t script_len);

/**
 * Set the witness stack for an input in a transaction.
 *
 * :param tx: The transaction to operate on.
 * :param index: The zero-based index of the input to set the witness stack on.
 * :param stack: The transaction witness stack to set.
 */

WALLY_CORE_API int wally_tx_set_input_witness(
    const struct wally_tx *tx,
    size_t index,
    const struct wally_tx_witness_stack *stack);

/**
 * Add a transaction output to a transaction.
 *
 * :param tx: The transaction to add the input to.
 * :param output: The transaction output to add to ``tx``.
 */
WALLY_CORE_API int wally_tx_add_output(
    struct wally_tx *tx,
    const struct wally_tx_output *output);

/**
 * Add a transaction output to a transaction.
 *
 * :param tx: The transaction to add the input to.
 * :param satoshi: The amount of the output in satoshi.
 * :param script: The scriptPubkey for the output.
 * :param script_len: Size of ``script`` in bytes.
 * :param flags: Flags controlling script creation. Must be 0.
 */
WALLY_CORE_API int wally_tx_add_raw_output(
    struct wally_tx *tx,
    uint64_t satoshi,
    const unsigned char *script,
    size_t script_len,
    uint32_t flags);

/**
 * Remove a transaction output from a transaction.
 *
 * :param tx: The transaction to remove the output from.
 * :param index: The zero-based index of the output to remove.
 */
WALLY_CORE_API int wally_tx_remove_output(
    struct wally_tx *tx,
    size_t index);

/**
 * Get the number of inputs in a transaction that have witness data.
 *
 * :param tx: The transaction to remove the output from.
 * :param written: Destination for the number of witness-containing inputs.
 */
WALLY_CORE_API int wally_tx_get_witness_count(
    const struct wally_tx *tx,
    size_t *written);

#ifndef SWIG_PYTHON
/**
 * Free a transaction allocated by `wally_tx_init_alloc`.
 *
 * :param tx: The transaction to free.
 */
WALLY_CORE_API int wally_tx_free(struct wally_tx *tx);
#endif /* SWIG_PYTHON */

/**
 * Return the length of transaction once serialized into bytes.
 *
 * :param tx: The transaction to find the serialized length of.
 * :param flags: WALLY_TX_FLAG_ Flags controlling serialization options.
 * :param written: Destination for the length of the serialized bytes.
 */
WALLY_CORE_API int wally_tx_get_length(
    const struct wally_tx *tx,
    uint32_t flags,
    size_t *written);

/**
 * Create a transaction from its serialized bytes.
 *
 * :param bytes: Bytes to create the transaction from.
 * :param bytes_len: Length of ``bytes`` in bytes.
 * :param flags: Flags controlling serialization options. Must be 0.
 * :param output: Destination for the resulting transaction.
 */
WALLY_CORE_API int wally_tx_from_bytes(
    const unsigned char *bytes,
    size_t bytes_len,
    uint32_t flags,
    struct wally_tx **output);

/**
 * Create a transaction from its serialized bytes in hexadecimal.
 *
 * :param hex: Hexadecimal string containing the transaction.
 * :param flags: Flags controlling serialization options. Must be 0.
 * :param output: Destination for the resulting transaction.
 */
WALLY_CORE_API int wally_tx_from_hex(
    const char *hex,
    uint32_t flags,
    struct wally_tx **output);

/**
 * Serialize a transaction to bytes.
 *
 * :param tx: The transaction to serialize.
 * :param flags: WALLY_TX_FLAG_ Flags controlling serialization options.
 * :param bytes_out: Destination for the serialized transaction.
 * :param len: Size of ``bytes_out`` in bytes.
 * :param written: Destination for the length of the serialized transaction.
 */
WALLY_CORE_API int wally_tx_to_bytes(
    const struct wally_tx *tx,
    uint32_t flags,
    unsigned char *bytes_out,
    size_t len,
    size_t *written);

/**
 * Serialize a transaction to hex.
 *
 * :param tx: The transaction to serialize.
 * :param flags: WALLY_TX_FLAG_ Flags controlling serialization options.
 * :param output: Destination for the resulting hexadecimal string.
 *
 * .. note:: The string returned should be freed using `wally_free_string`.
 */
WALLY_CORE_API int wally_tx_to_hex(
    const struct wally_tx *tx,
    uint32_t flags,
    char **output);

/**
 * Get the weight of a transaction.
 *
 * :param tx: The transaction to get the weight of.
 * :param written: Destination for the weight.
 */
WALLY_CORE_API int wally_tx_get_weight(
    const struct wally_tx *tx,
    size_t *written);

/**
 * Get the virtual size of a transaction.
 *
 * :param tx: The transaction to get the virtual size of.
 * :param written: Destination for the virtual size.
 */
WALLY_CORE_API int wally_tx_get_vsize(
    const struct wally_tx *tx,
    size_t *written);

/**
 * Compute transaction vsize from transaction weight.
 *
 * :param weight: The weight to convert to a virtual size.
 * :param written: Destination for the virtual size.
 */
WALLY_CORE_API int wally_tx_vsize_from_weight(
    size_t weight,
    size_t *written);

/**
 * Create a BTC transaction for signing and return its hash.
 *
 * :param tx: The transaction to generate the signature hash from.
 * :param index: The input index of the input being signed for.
 * :param script: The scriptSig for the input represented by ``index``.
 * :param script_len: Size of ``script`` in bytes.
 * :param satoshi: The amount spent by the input being signed for. Only used if
 *|     flags includes WALLY_TX_FLAG_USE_WITNESS, pass 0 otherwise.
 * :param sighash: WALLY_SIGHASH_ flags specifying the type of signature desired.
 * :param flags: WALLY_TX_FLAG_USE_WITNESS to generate a BIP 143 signature, or 0
 *|     to generate a pre-segwit Bitcoin signature.
 * :param bytes_out: Destination for the signature.
 * :param len: Size of ``bytes_out`` in bytes. Must be at least ``SHA256_LEN``.
 */
WALLY_CORE_API int wally_tx_get_btc_signature_hash(
    const struct wally_tx *tx,
    size_t index,
    const unsigned char *script,
    size_t script_len,
    uint64_t satoshi,
    uint32_t sighash,
    uint32_t flags,
    unsigned char *bytes_out,
    size_t len);

/**
 * Create a transaction for signing and return its hash.
 *
 * :param tx: The transaction to generate the signature hash from.
 * :param index: The input index of the input being signed for.
 * :param script: The scriptSig for the input represented by ``index``.
 * :param script_len: Size of ``script`` in bytes.
 * :param extra: Extra bytes to include in the transaction preimage.
 * :param extra_len: Size of ``extra`` in bytes.
 * :param extra_offset: Offset with the preimage to store ``extra``. To store
 *|     it and the end of the preimage, use 0xffffffff.
 * :param satoshi: The amount spent by the input being signed for. Only used if
 *|     flags includes WALLY_TX_FLAG_USE_WITNESS, pass 0 otherwise.
 * :param sighash: WALLY_SIGHASH_ flags specifying the type of signature desired.
 * :param tx_sighash: The 32bit sighash value to include in the preimage to hash.
 *|     This must be given in host CPU endianess; For normal Bitcoin signing
 *|     the value of ``sighash`` should be given.
 * :param flags: WALLY_TX_FLAG_USE_WITNESS to generate a BIP 143 signature, or 0
 *|     to generate a pre-segwit Bitcoin signature.
 * :param bytes_out: Destination for the signature.
 * :param len: Size of ``bytes_out`` in bytes. Must be at least ``SHA256_LEN``.
 */
WALLY_CORE_API int wally_tx_get_signature_hash(
    const struct wally_tx *tx,
    size_t index,
    const unsigned char *script,
    size_t script_len,
    const unsigned char *extra,
    size_t extra_len,
    uint32_t extra_offset,
    uint64_t satoshi,
    uint32_t sighash,
    uint32_t tx_sighash,
    uint32_t flags,
    unsigned char *bytes_out,
    size_t len);

#ifdef __cplusplus
}
#endif

#endif /* LIBWALLY_CORE_TRANSACTION_H */