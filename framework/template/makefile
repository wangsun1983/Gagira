templateDir = ./framework/template

objs += $(outtmp)/template.o \
		$(outtmp)/templateinstruction.o \
		$(outtmp)/templateparserdispatcher.o \
		$(outtmp)/templateitem.o \
		$(outtmp)/templatecommentitem.o \
		$(outtmp)/templatetextitem.o  \
		$(outtmp)/templatefunctionitem.o  \
		$(outtmp)/templateprintitem.o  \
		$(outtmp)/templateletitem.o  \
		$(outtmp)/templatescopedvalue.o  \
		$(outtmp)/templatecommentparser.o  \
		$(outtmp)/templateletparser.o  \
		$(outtmp)/templatefunctionparser.o   \
		$(outtmp)/templateprintparser.o   

$(outtmp)/template.o :$(templateDir)/Template.cpp
	$(cppcompiler) $(gagiracppflags) -c $(templateDir)/Template.cpp -o $(outtmp)/template.o

$(outtmp)/templateinstruction.o :$(templateDir)/TemplateInstruction.cpp
	$(cppcompiler) $(gagiracppflags) -c $(templateDir)/TemplateInstruction.cpp -o $(outtmp)/templateinstruction.o

#---------- item -----------
$(outtmp)/templateitem.o :$(templateDir)/item/TemplateItem.cpp
	$(cppcompiler) $(gagiracppflags) -c $(templateDir)/item/TemplateItem.cpp -o $(outtmp)/templateitem.o

$(outtmp)/templatescopedvalue.o :$(templateDir)/item/TemplateScopedValue.cpp
	$(cppcompiler) $(gagiracppflags) -c $(templateDir)/item/TemplateScopedValue.cpp -o $(outtmp)/templatescopedvalue.o

$(outtmp)/templatefunctionitem.o :$(templateDir)/item/TemplateFunctionItem.cpp
	$(cppcompiler) $(gagiracppflags) -c $(templateDir)/item/TemplateFunctionItem.cpp -o $(outtmp)/templatefunctionitem.o

$(outtmp)/templatetextitem.o :$(templateDir)/item/TemplateTextItem.cpp
	$(cppcompiler) $(gagiracppflags) -c $(templateDir)/item/TemplateTextItem.cpp -o $(outtmp)/templatetextitem.o

$(outtmp)/templateletitem.o :$(templateDir)/item/TemplateLetItem.cpp
	$(cppcompiler) $(gagiracppflags) -c $(templateDir)/item/TemplateLetItem.cpp -o $(outtmp)/templateletitem.o

$(outtmp)/templateprintitem.o :$(templateDir)/item/TemplatePrintItem.cpp
	$(cppcompiler) $(gagiracppflags) -c $(templateDir)/item/TemplatePrintItem.cpp -o $(outtmp)/templateprintitem.o

$(outtmp)/templatecommentitem.o :$(templateDir)/item/TemplateCommentItem.cpp
	$(cppcompiler) $(gagiracppflags) -c $(templateDir)/item/TemplateCommentItem.cpp -o $(outtmp)/templatecommentitem.o

#----------- parser -----------
$(outtmp)/templatecommentparser.o :$(templateDir)/parser/TemplateCommentParser.cpp
	$(cppcompiler) $(gagiracppflags) -c $(templateDir)/parser/TemplateCommentParser.cpp -o $(outtmp)/templatecommentparser.o

$(outtmp)/templateletparser.o :$(templateDir)/parser/TemplateLetParser.cpp
	$(cppcompiler) $(gagiracppflags) -c $(templateDir)/parser/TemplateLetParser.cpp -o $(outtmp)/templateletparser.o

$(outtmp)/templatefunctionparser.o :$(templateDir)/parser/TemplateFunctionParser.cpp
	$(cppcompiler) $(gagiracppflags) -c $(templateDir)/parser/TemplateFunctionParser.cpp -o $(outtmp)/templatefunctionparser.o

$(outtmp)/templateprintparser.o :$(templateDir)/parser/TemplatePrintParser.cpp
	$(cppcompiler) $(gagiracppflags) -c $(templateDir)/parser/TemplatePrintParser.cpp -o $(outtmp)/templateprintparser.o

$(outtmp)/templateparserdispatcher.o :$(templateDir)/parser/TemplateParserDispatcher.cpp
	$(cppcompiler) $(gagiracppflags) -c $(templateDir)/parser/TemplateParserDispatcher.cpp -o $(outtmp)/templateparserdispatcher.o
