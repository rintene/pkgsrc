# $NetBSD: buildlink3.mk,v 1.1 2023/11/22 14:06:50 ryoon Exp $

BUILDLINK_TREE+=	openjdk25

.if !defined(OPENJDK25_BUILDLINK3_MK)
OPENJDK25_BUILDLINK3_MK:=

BUILDLINK_LIBDIRS.openjdk25+=	java/openjdk25/lib
BUILDLINK_LIBDIRS.openjdk25+=	java/openjdk25/lib/server

BUILDLINK_API_DEPENDS.openjdk25+=	openjdk25>=1.21.0.1.12
BUILDLINK_PKGSRCDIR.openjdk2?=		./

.endif	# OPENJDK21_BUILDLINK3_MK

BUILDLINK_TREE+=	-openjdk25
