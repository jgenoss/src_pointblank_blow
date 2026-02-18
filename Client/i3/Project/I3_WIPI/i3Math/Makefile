I3_PRJ_DIR	=	$(I3_WIPI_ROOT)/Project/I3_WIPI
include			$(I3_PRJ_DIR)/Common/makedef

TARGET		=	$(OUTDIR)/libi3Math.a
OBJS		=	i3MathType.o i3Math.o i3Quat.o i3Matrix.o \
				i3MatrixStackWIPI.o i3MatrixArray.o i3MatrixObject.o i3MatrixObjectList.o \
				i3TransformSource.o i3TransformSequence.o i3TransformSourceBind.o i3TransformSourceCombiner.o \
				i3Volume.o i3BoundBox.o i3RectCoordSource.o \
				i3Vector3Array.o i3Vector2Array.o i3Vector4Array.o i3Vector.o i3Key.o \
				i3MathGlobalVariable.o i3MathUtil.o i3MathMeta.o \
				i3KeyframeTable.o i3ParticleInfo.o i3FixedPointMatrix.o i3FixedPointQuat.o

INCS		=	-I$(I3MATH_INC)

include			$(I3_PRJ_DIR)/Common/makebase

vpath %.cpp		$(I3MATH_SRC)
