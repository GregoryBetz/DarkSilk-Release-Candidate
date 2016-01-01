// Copyright (c) 2009-2016 Satoshi Nakamoto
// Copyright (c) 2009-2016 The Bitcoin Developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef DARKSILK_PRIMITIVES_TRANSACTION_H
#define DARKSILK_PRIMITIVES_TRANSACTION_H

#include "uint256.h"
#include "serialize.h"
#include "util.h"
#include "script.h"
#include "timedata.h"

#include <stdio.h>


/// The maximum allowed size for a serialized block, in bytes (network rule)
static const unsigned int MAX_BLOCK_SIZE = 50000000; // 50MB Maximum Block Size (50x Bitcoin Core)
/// No amount larger than this (in satoshi) is valid
static const int64_t MAX_MONEY = 90000000 * COIN; // 45,000,000 instamined from blocks 1 & 2 for Weaver Collateral (main.cpp lines 1129-1143) | 1,679,958 PoW Generated Coins

inline bool MoneyRange(int64_t nValue) { return (nValue >= 0 && nValue <= MAX_MONEY); }

using namespace std;

/// An outpoint - a combination of a transaction hash and an index n into its vout
class COutPoint
{
public:
    uint256 hash;
    unsigned int n;

    COutPoint() { SetNull(); }
    COutPoint(uint256 hashIn, unsigned int nIn) { hash = hashIn; n = nIn; }

    IMPLEMENT_SERIALIZE( READWRITE(FLATDATA(*this)); )
    void SetNull() { hash = 0; n = (unsigned int) -1; }
    bool IsNull() const { return (hash == 0 && n == (unsigned int) -1); }

    friend bool operator<(const COutPoint& a, const COutPoint& b)
    {
        return (a.hash < b.hash || (a.hash == b.hash && a.n < b.n));
    }

    friend bool operator==(const COutPoint& a, const COutPoint& b)
    {
        return (a.hash == b.hash && a.n == b.n);
    }

    friend bool operator!=(const COutPoint& a, const COutPoint& b)
    {
        return !(a == b);
    }

    std::string ToString() const
    {
        return strprintf("COutPoint(%s, %u)", hash.ToString().substr(0,10), n);
    }

    std::string ToStringShort() const
    {
        return strprintf("%s-%u", hash.ToString().substr(0,64), n);
    }

    uint256 GetHash() const
    {
        return SerializeHash(*this);
    }

};

///  An inpoint - a combination of a transaction and an index n into its vin
class CInPoint
{
public:
    CTransaction* ptx;
    unsigned int n;

    CInPoint() { SetNull(); }
    CInPoint(CTransaction* ptxIn, unsigned int nIn) { ptx = ptxIn; n = nIn; }
    void SetNull() { ptx = NULL; n = (unsigned int) -1; }
    bool IsNull() const { return (ptx == NULL && n == (unsigned int) -1); }
};

///  An input of a transaction.  It contains the location of the previous
///  transaction's output that it claims and a signature that matches the
///  output's public key.
class CTxIn
{
public:
    COutPoint prevout;
    CScript scriptSig;
    CScript prevPubKey;
    unsigned int nSequence;

    CTxIn()
    {
        nSequence = std::numeric_limits<unsigned int>::max();
    }

    explicit CTxIn(COutPoint prevoutIn, CScript scriptSigIn=CScript(), unsigned int nSequenceIn=std::numeric_limits<unsigned int>::max())
    {
        prevout = prevoutIn;
        scriptSig = scriptSigIn;
        nSequence = nSequenceIn;
    }

    explicit CTxIn(uint256 hashPrevTx, unsigned int nOut, CScript scriptSigIn=CScript(), unsigned int nSequenceIn=std::numeric_limits<unsigned int>::max())
    {
        prevout = COutPoint(hashPrevTx, nOut);
        scriptSig = scriptSigIn;
        nSequence = nSequenceIn;
    }

    IMPLEMENT_SERIALIZE
    (
        READWRITE(prevout);
        READWRITE(scriptSig);
        READWRITE(nSequence);
    )

    bool IsFinal() const
    {
        return (nSequence == std::numeric_limits<unsigned int>::max());
    }

    friend bool operator==(const CTxIn& a, const CTxIn& b)
    {
        return (a.prevout   == b.prevout &&
                a.scriptSig == b.scriptSig &&
                a.nSequence == b.nSequence);
    }

    friend bool operator!=(const CTxIn& a, const CTxIn& b)
    {
        return !(a == b);
    }

    std::string ToString() const
    {
        std::string str;
        str += "CTxIn(";
        str += prevout.ToString();
        if (prevout.IsNull())
            str += strprintf(", coinbase %s", HexStr(scriptSig));
        else
            str += strprintf(", scriptSig=%s", scriptSig.ToString().substr(0,24));
        if (nSequence != std::numeric_limits<unsigned int>::max())
            str += strprintf(", nSequence=%u", nSequence);
        str += ")";
        return str;
    }
};

///  An output of a transaction.  It contains the public key that the next input
///  must be able to sign with to claim it.
class CTxOut
{
public:
    int64_t nValue;
    int nRounds;
    CScript scriptPubKey;

    CTxOut()
    {
        SetNull();
    }

    CTxOut(int64_t nValueIn, CScript scriptPubKeyIn)
    {
        nValue = nValueIn;
        nRounds = -10; // an initial value, should be no way to get this by calculations
        scriptPubKey = scriptPubKeyIn;
    }

    IMPLEMENT_SERIALIZE
    (
        READWRITE(nValue);
        READWRITE(scriptPubKey);
    )

    void SetNull()
    {
        nValue = -1;
        nRounds = -10; // an initial value, should be no way to get this by calculations
        scriptPubKey.clear();
    }

    bool IsNull() const
    {
        return (nValue == -1);
    }

    void SetEmpty()
    {
        nValue = 0;
        scriptPubKey.clear();
    }

    bool IsEmpty() const
    {
        return (nValue == 0 && scriptPubKey.empty());
    }

    uint256 GetHash() const
    {
        return SerializeHash(*this);
    }

    bool IsDust(int64_t MIN_RELAY_TX_FEE) const
    {
        // "Dust" is defined in terms of MIN_RELAY_TX_FEE,
        // which has units satoshis-per-kilobyte.
        // If you'd pay more than 1/3 in fees
        // to spend something, then we consider it dust.
        // A typical txout is 34 bytes big, and will
        // need a CTxIn of at least 148 bytes to spend,
        // so dust is a txout less than 546 satoshis
        // with default nMinRelayTxFee.
        return ((nValue*1000)/(3*((int)GetSerializeSize(SER_DISK,0)+148)) < MIN_RELAY_TX_FEE);
    }

    friend bool operator==(const CTxOut& a, const CTxOut& b)
    {
        return (a.nValue       == b.nValue &&
                a.nRounds      == b.nRounds &&
                a.scriptPubKey == b.scriptPubKey);
    }

    friend bool operator!=(const CTxOut& a, const CTxOut& b)
    {
        return !(a == b);
    }

    std::string ToString() const
    {
        if (IsEmpty()) return "CTxOut(empty)";
        return strprintf("CTxOut(nValue=%s, scriptPubKey=%s)", FormatMoney(nValue), scriptPubKey.ToString());
    }
};


/// The basic transaction that is broadcasted on the network and contained in
/// blocks.  A transaction can contain multiple inputs and outputs.
class CTransaction
{
public:
    static const int CURRENT_VERSION=1;
    int nVersion;
    unsigned int nTime;
    std::vector<CTxIn> vin;
    std::vector<CTxOut> vout;
    unsigned int nLockTime;

    // Denial-of-service detection:
    mutable int nDoS;
    bool DoS(int nDoSIn, bool fIn) const { nDoS += nDoSIn; return fIn; }

    CTransaction()
    {
        SetNull();
    }

    CTransaction(int nVersion, unsigned int nTime, const std::vector<CTxIn>& vin, const std::vector<CTxOut>& vout, unsigned int nLockTime)
        : nVersion(nVersion), nTime(nTime), vin(vin), vout(vout), nLockTime(nLockTime), nDoS(0)
    {
    }

    /// Convert a CMutableTransaction into a CTransaction.
    CTransaction(const CMutableTransaction &tx);

    IMPLEMENT_SERIALIZE
    (
        READWRITE(this->nVersion);
        nVersion = this->nVersion;
        READWRITE(nTime);
        READWRITE(vin);
        READWRITE(vout);
        READWRITE(nLockTime);
    )

    void SetNull()
    {
        nVersion = CTransaction::CURRENT_VERSION;
        nTime = GetAdjustedTime();
        vin.clear();
        vout.clear();
        nLockTime = 0;
        nDoS = 0;  // Denial-of-service prevention
    }

    bool IsNull() const
    {
        return (vin.empty() && vout.empty());
    }

    uint256 GetHash() const
    {
        return SerializeHash(*this); //TODO (Amir): Use a cached result (need member variables to const).
    }

    bool IsCoinBase() const
    {
        return (vin.size() == 1 && vin[0].prevout.IsNull() && vout.size() >= 1);
    }

    bool IsCoinStake() const
    {
        // ppcoin: the coin stake transaction is marked with the first output empty
        return (vin.size() > 0 && (! vin[0].prevout.IsNull()) && vout.size() >= 2 && vout[0].IsEmpty());
    }

    // Return sum of txouts.
    CAmount GetValueOut() const;
    // Compute priority, given priority of inputs and (optionally) tx size
    double ComputePriority(double dPriorityInputs, unsigned int nTxSize=0) const;

    friend bool operator==(const CTransaction& a, const CTransaction& b)
    {
        //TODO (Amir): Use a cached hash (need member variables to const).
        return (a.nVersion  == b.nVersion &&
                a.nTime     == b.nTime &&
                a.vin       == b.vin &&
                a.vout      == b.vout &&
                a.nLockTime == b.nLockTime);
    }

    friend bool operator!=(const CTransaction& a, const CTransaction& b)
    {
        return !(a == b);
    }

    bool CheckTransaction() const;

    std::string ToString() const;
};

/// A mutable version of CTransaction.
struct CMutableTransaction
{
    static const int32_t CURRENT_VERSION = 1;
    int32_t nVersion;
    uint32_t nTime;
    std::vector<CTxIn> vin;
    std::vector<CTxOut> vout;
    uint32_t nLockTime;

    // Denial-of-service detection:
    mutable int32_t nDoS;
    bool DoS(int32_t nDoSIn, bool fIn) const { nDoS += nDoSIn; return fIn; }

    CMutableTransaction();
    CMutableTransaction(const CTransaction& tx);

    CMutableTransaction(int32_t nVersion, uint32_t nTime, const std::vector<CTxIn>& vin, const std::vector<CTxOut>& vout, uint32_t nLockTime)
        : nVersion(nVersion), nTime(nTime), vin(vin), vout(vout), nLockTime(nLockTime), nDoS(0)
    {
    }

    IMPLEMENT_SERIALIZE
    (
        READWRITE(this->nVersion);
        nVersion = this->nVersion;
        READWRITE(this->nTime);
        READWRITE(vin);
        READWRITE(vout);
        READWRITE(nLockTime);
    )

    /// Compute the hash of this CMutableTransaction. This is computed on the
    /// fly, as opposed to GetHash() in CTransaction, which uses a cached result.
    uint256 GetHash() const;

    std::string ToString() const;

    bool IsNull() const
    {
        return (vin.empty() && vout.empty());
    }

    friend bool operator==(const CMutableTransaction& a, const CMutableTransaction& b)
    {
        return (a.nVersion  == b.nVersion &&
                a.nTime     == b.nTime &&
                a.vin       == b.vin &&
                a.vout      == b.vout &&
                a.nLockTime == b.nLockTime);
    }

    friend bool operator!=(const CMutableTransaction& a, const CMutableTransaction& b)
    {
        return !(a == b);
    }

};

class TransactionSignatureChecker : public BaseSignatureChecker
{
private:
    const CTransaction* txTo;
    unsigned int nIn;

protected:
    virtual bool VerifySignature(const std::vector<unsigned char>& vchSig, const CPubKey& vchPubKey, const uint256& sighash) const;

public:
    TransactionSignatureChecker(const CTransaction* txToIn, unsigned int nInIn) : txTo(txToIn), nIn(nInIn) {}
    bool CheckSig(const std::vector<unsigned char>& scriptSig, const std::vector<unsigned char>& vchPubKey, const CScript& scriptCode) const;
};

class MutableTransactionSignatureChecker : public TransactionSignatureChecker
{
private:
    const CTransaction txTo;

public:
    MutableTransactionSignatureChecker(const CMutableTransaction* txToIn, unsigned int nInIn) : TransactionSignatureChecker(&txTo, nInIn), txTo(*txToIn) {}
};

#endif // DARKSILK_PRIMITIVES_TRANSACTION_H