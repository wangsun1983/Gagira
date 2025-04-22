templateDir = ./framework/template

objs += $(outtmp)/template.o \
		$(outtmp)/templateparserhelper.o \
		$(outtmp)/templatescopedvaluehelper.o \
		$(outtmp)/templatescopedvaluecontainer.o \
		$(outtmp)/templateinstruction.o \
		$(outtmp)/templateobjectcontainer.o \
		$(outtmp)/templateparserdispatcher.o \
		$(outtmp)/templateitem.o \
		$(outtmp)/templateinitem.o \
		$(outtmp)/templatecommentitem.o \
		$(outtmp)/templatereturnitem.o \
		$(outtmp)/templatebreakitem.o \
		$(outtmp)/templatetextitem.o  \
		$(outtmp)/templatefunctionitem.o  \
		$(outtmp)/templateconditionitem.o  \
		$(outtmp)/templateprintitem.o  \
		$(outtmp)/templateletitem.o  \
		$(outtmp)/templateloopitem.o  \
		$(outtmp)/templatescopedvalue.o  \
		$(outtmp)/templateparser.o  \
		$(outtmp)/templateinparser.o  \
		$(outtmp)/templatereturnparser.o  \
		$(outtmp)/templatebreakparser.o  \
		$(outtmp)/templatecommentparser.o  \
		$(outtmp)/templatecontionparser.o  \
		$(outtmp)/templateletparser.o  \
		$(outtmp)/templatefunctionparser.o   \
		$(outtmp)/templateprintparser.o   \
		$(outtmp)/templateelseparser.o   \
		$(outtmp)/templateloopparser.o   \
		$(outtmp)/templateendparser.o   \

$(outtmp)/template.o :$(templateDir)/Template.cpp
	$(cppcompiler) $(gagiracppflags) -c $(templateDir)/Template.cpp -o $(outtmp)/template.o

$(outtmp)/templateparserhelper.o :$(templateDir)/TemplateParserHelper.cpp
	$(cppcompiler) $(gagiracppflags) -c $(templateDir)/TemplateParserHelper.cpp -o $(outtmp)/templateparserhelper.o

$(outtmp)/templatescopedvaluehelper.o :$(templateDir)/TemplateScopedValueHelper.cpp
	$(cppcompiler) $(gagiracppflags) -c $(templateDir)/TemplateScopedValueHelper.cpp -o $(outtmp)/templatescopedvaluehelper.o

$(outtmp)/templatescopedvaluecontainer.o :$(templateDir)/TemplateScopedValueContainer.cpp
	$(cppcompiler) $(gagiracppflags) -c $(templateDir)/TemplateScopedValueContainer.cpp -o $(outtmp)/templatescopedvaluecontainer.o

$(outtmp)/templateobjectcontainer.o :$(templateDir)/TemplateObjectContainer.cpp
	$(cppcompiler) $(gagiracppflags) -c $(templateDir)/TemplateObjectContainer.cpp -o $(outtmp)/templateobjectcontainer.o

$(outtmp)/templateinstruction.o :$(templateDir)/TemplateInstruction.cpp
	$(cppcompiler) $(gagiracppflags) -c $(templateDir)/TemplateInstruction.cpp -o $(outtmp)/templateinstruction.o
#---------- item -----------
$(outtmp)/templateitem.o :$(templateDir)/item/TemplateItem.cpp
	$(cppcompiler) $(gagiracppflags) -c $(templateDir)/item/TemplateItem.cpp -o $(outtmp)/templateitem.o

$(outtmp)/templateinitem.o :$(templateDir)/item/TemplateInItem.cpp
	$(cppcompiler) $(gagiracppflags) -c $(templateDir)/item/TemplateInItem.cpp -o $(outtmp)/templateinitem.o

$(outtmp)/templateniitem.o :$(templateDir)/item/TemplateNiItem.cpp
	$(cppcompiler) $(gagiracppflags) -c $(templateDir)/item/TemplateNiItem.cpp -o $(outtmp)/templateniitem.o

$(outtmp)/templatescopedvalue.o :$(templateDir)/item/TemplateScopedValue.cpp
	$(cppcompiler) $(gagiracppflags) -c $(templateDir)/item/TemplateScopedValue.cpp -o $(outtmp)/templatescopedvalue.o

$(outtmp)/templatefunctionitem.o :$(templateDir)/item/TemplateFunctionItem.cpp
	$(cppcompiler) $(gagiracppflags) -c $(templateDir)/item/TemplateFunctionItem.cpp -o $(outtmp)/templatefunctionitem.o

$(outtmp)/templateconditionitem.o :$(templateDir)/item/TemplateConditionItem.cpp
	$(cppcompiler) $(gagiracppflags) -c $(templateDir)/item/TemplateConditionItem.cpp -o $(outtmp)/templateconditionitem.o

$(outtmp)/templatetextitem.o :$(templateDir)/item/TemplateTextItem.cpp
	$(cppcompiler) $(gagiracppflags) -c $(templateDir)/item/TemplateTextItem.cpp -o $(outtmp)/templatetextitem.o

$(outtmp)/templateletitem.o :$(templateDir)/item/TemplateLetItem.cpp
	$(cppcompiler) $(gagiracppflags) -c $(templateDir)/item/TemplateLetItem.cpp -o $(outtmp)/templateletitem.o

$(outtmp)/templateloopitem.o :$(templateDir)/item/TemplateLoopItem.cpp
	$(cppcompiler) $(gagiracppflags) -c $(templateDir)/item/TemplateLoopItem.cpp -o $(outtmp)/templateloopitem.o

$(outtmp)/templateprintitem.o :$(templateDir)/item/TemplatePrintItem.cpp
	$(cppcompiler) $(gagiracppflags) -c $(templateDir)/item/TemplatePrintItem.cpp -o $(outtmp)/templateprintitem.o

$(outtmp)/templatecommentitem.o :$(templateDir)/item/TemplateCommentItem.cpp
	$(cppcompiler) $(gagiracppflags) -c $(templateDir)/item/TemplateCommentItem.cpp -o $(outtmp)/templatecommentitem.o

$(outtmp)/templatereturnitem.o :$(templateDir)/item/TemplateReturnItem.cpp
	$(cppcompiler) $(gagiracppflags) -c $(templateDir)/item/TemplateReturnItem.cpp -o $(outtmp)/templatereturnitem.o

$(outtmp)/templatebreakitem.o :$(templateDir)/item/TemplateBreakItem.cpp
	$(cppcompiler) $(gagiracppflags) -c $(templateDir)/item/TemplateBreakItem.cpp -o $(outtmp)/templatebreakitem.o

#----------- parser -----------
$(outtmp)/templateparser.o :$(templateDir)/parser/TemplateParser.cpp
	$(cppcompiler) $(gagiracppflags) -c $(templateDir)/parser/TemplateParser.cpp -o $(outtmp)/templateparser.o

$(outtmp)/templatecommentparser.o :$(templateDir)/parser/TemplateCommentParser.cpp
	$(cppcompiler) $(gagiracppflags) -c $(templateDir)/parser/TemplateCommentParser.cpp -o $(outtmp)/templatecommentparser.o

$(outtmp)/templatecontionparser.o :$(templateDir)/parser/TemplateConditionParser.cpp
	$(cppcompiler) $(gagiracppflags) -c $(templateDir)/parser/TemplateConditionParser.cpp -o $(outtmp)/templatecontionparser.o

$(outtmp)/templateletparser.o :$(templateDir)/parser/TemplateLetParser.cpp
	$(cppcompiler) $(gagiracppflags) -c $(templateDir)/parser/TemplateLetParser.cpp -o $(outtmp)/templateletparser.o

$(outtmp)/templatefunctionparser.o :$(templateDir)/parser/TemplateFunctionParser.cpp
	$(cppcompiler) $(gagiracppflags) -c $(templateDir)/parser/TemplateFunctionParser.cpp -o $(outtmp)/templatefunctionparser.o

$(outtmp)/templateprintparser.o :$(templateDir)/parser/TemplatePrintParser.cpp
	$(cppcompiler) $(gagiracppflags) -c $(templateDir)/parser/TemplatePrintParser.cpp -o $(outtmp)/templateprintparser.o

$(outtmp)/templateelseparser.o :$(templateDir)/parser/TemplateElseParser.cpp
	$(cppcompiler) $(gagiracppflags) -c $(templateDir)/parser/TemplateElseParser.cpp -o $(outtmp)/templateelseparser.o

$(outtmp)/templatefiparser.o :$(templateDir)/parser/TemplateFiParser.cpp
	$(cppcompiler) $(gagiracppflags) -c $(templateDir)/parser/TemplateFiParser.cpp -o $(outtmp)/templatefiparser.o

$(outtmp)/templateloopparser.o :$(templateDir)/parser/TemplateLoopParser.cpp
	$(cppcompiler) $(gagiracppflags) -c $(templateDir)/parser/TemplateLoopParser.cpp -o $(outtmp)/templateloopparser.o

$(outtmp)/templateinparser.o :$(templateDir)/parser/TemplateInParser.cpp
	$(cppcompiler) $(gagiracppflags) -c $(templateDir)/parser/TemplateInParser.cpp -o $(outtmp)/templateinparser.o

$(outtmp)/templatereturnparser.o :$(templateDir)/parser/TemplateReturnParser.cpp
	$(cppcompiler) $(gagiracppflags) -c $(templateDir)/parser/TemplateReturnParser.cpp -o $(outtmp)/templatereturnparser.o

$(outtmp)/templatebreakparser.o :$(templateDir)/parser/TemplateBreakParser.cpp
	$(cppcompiler) $(gagiracppflags) -c $(templateDir)/parser/TemplateBreakParser.cpp -o $(outtmp)/templatebreakparser.o

$(outtmp)/templateendparser.o :$(templateDir)/parser/TemplateEndParser.cpp
	$(cppcompiler) $(gagiracppflags) -c $(templateDir)/parser/TemplateEndParser.cpp -o $(outtmp)/templateendparser.o

$(outtmp)/templateparserdispatcher.o :$(templateDir)/parser/TemplateParserDispatcher.cpp
	$(cppcompiler) $(gagiracppflags) -c $(templateDir)/parser/TemplateParserDispatcher.cpp -o $(outtmp)/templateparserdispatcher.o
