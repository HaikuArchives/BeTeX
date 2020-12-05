\"\\$ " << "TEXMFOUTPUT=/tmp " << command.String() << "\"";
			echostr.ReplaceAll("\n","");
			echostr << "\n";

			BString end;
			end << "echo \"(Press Enter To Continue)\"\nread\nexit 0\n";

			BFile file(script,B_READ_WRITE | B_CREATE_FILE | B_ERASE_FILE);

			if(file.InitCheck() == B_OK)
			{
				file.Write(initial.String(),initial.Length());
				file.Write(echostr.String(),echostr.Length()); 
				file.Write(command.String(),command.Length());
				file.Write(end.String(),end.Length());
				file.SetPermissions(X_OK);
				file.Unset();
				const char *args[] = {script, NULL};
				//TODO : for BeOS R5, the signature is x-vnd.Be-SHEL, how to deal both Haiku and BeOS ?
				be_roster->Launch("application/x-vnd.Haiku-Terminal",1,args);

			}
		}
	}
	ResetPermissions();
}

void MainWindow::ResetPermissions()
{
	TexView* tv = CurrentTexView();
	ProjectItem* li = CurrentTListItem();
	//replace by loops where possible!
	if(tv == NULL)
	{
		//disable those things not used......
		//edit items
		fclose->SetEnabled(false);
		fsave->SetEnabled(false);
		fsaveas->SetEnabled(false);
		fpgsetup->SetEnabled(false);
		fprint->SetEnabled(false);
		//edit items
		fundo->SetEnabled(false);
		//fredo->SetEnabled(false);

		fbold->SetEnabled(false);
		femph->SetEnabled(false);
		fital->SetEnabled(false);
		fshiftleft->SetEnabled(false);
		fshiftright->SetEnabled(false);



		fcut->SetEnabled(false);
		fcopy->SetEnabled(false);
		fpaste->SetEnabled(false);
		fselall->SetEnabled(false);
		fsearchreplace->SetEnabled(false);
		fgotoline->SetEnabled(false);
		fcomment->SetEnabled(false);
		funcomment->SetEnabled(false);

		ftexdvi->SetEnabled(false);
		fdvipdf->SetEnabled(false);
		fdvips->SetEnabled(false);
		fpspdf->SetEnabled(false);
		ftexpdf->SetEnabled(false);
		ftexhtml->SetEnabled(false);

		fpostscript->SetEnabled(false);
		fpdf->SetEnabled(false);
		fhtml->SetEnabled(false);

		m_toolBar->TBSave->SetEnabled(false);
		m_toolBar->TBPrint->SetEnabled(false);
		//m_toolBar->TBDelTmp->SetEnabled(false);
		m_toolBar->TBViewLog->SetEnabled(false);
		m_toolBar->TBTexDvi->SetEnabled(false);
		m_toolBar->TBDviPdf->SetEnabled(false);
		m_toolBar->TBDviPs->SetEnabled(false);
		m_toolBar->TBPsPdf->SetEnabled(false);
		m_toolBar->TBTexDvi->SetEnabled(false);
		m_toolBar->TBTexPdf->SetEnabled(false);
		m_toolBar->TBTexHtml->SetEnabled(false);
		m_toolBar->TBOpenTracker->SetEnabled(false);
		m_toolBar->TBOpenTerminal->SetEnabled(false);
		m_toolBar->TBPrevPDF->SetEnabled(false);
		m_toolBar->TBPrevPS->SetEnabled(false);
		m_toolBar->TBPrevHTML->SetEnabled(false);
		//m_toolBar->TBDelTmp->SetAuxIcon(true);


		finsertfile->SetEnabled(false);
		fdate->SetEnabled(false);
		farray->SetEnabled(false);
		fmatrix->SetEnabled(false);
		ftabular->SetEnabled(false);
		fequation->SetEnabled(false);
		frgbcolor->SetEnabled(false);
		flists->SetEnabled(false);
		flistsubmenu->SetEnabled(false);
		/*
		fitemize->SetEnabled(false);
		fdescription->SetEnabled(false);
		fenumerate->SetEnabled(false);
		*/
		//fcases->SetEnabled(false);
		fenvironments->SetEnabled(false);
		fenvironmentssubmenu->SetEnabled(false);
		/*ffigure->SetEnabled(false);
		ftable->SetEnabled(false);
		//fetabular->SetEnabled(false);
		//fetabularstar->SetEnabled(false);
		//ftabbing->SetEnabled(false);
		fcenter->SetEnabled(false);
		fflushleft->SetEnabled(false);
		fflushright->SetEnabled(false);
		feqnarray->SetEnabled(false);
		ffeqnarraystar->SetEnabled(false);
		fquote->SetEnabled(false);
		//fminipage->SetEnabled(false);
		*/
	}
	else
	{

		bool DoesPdfExist = false;
		bool DoesPsExist = false;
		bool DoesDviExist = false;
		bool DoesHtmlExist = false;
		bool DoesLogExist = false;
		bool DoesTexExist = li->IsHomely();

		bool IsSomeTextPasteable = false;

		bool IsSaveNeeded = li->IsSaveNeeded() || !DoesTexExist;

		bool IsNonZeroText = tv->TextLength() > 0;
		int32 start,finish;
		tv->GetSelection(&start,&finish);
		bool IsSomeTextSelected = (start != finish);

		BPath parent;
		entry_ref ref  = li->GetRef();
		TexPath.SetTo(&ref);
		if(TexPath.InitCheck() == B_OK)
		{
			basename = "";
			basename << TexPath.Leaf();
			basename.Replace(".tex","",1);


			TexPath.GetParent(&parent);
			parent_dir = parent;
		}

		if(DoesTexExist)
		{
			BString pdf_path;
			BString dvi_path;
			BString ps_path;
			BString html_path;
			BString log_path;

			pdf_path << TexPath.Path();
			pdf_path.Replace(".tex",".pdf",1);

			log_path << TexPath.Path();
			log_path.Replace(".tex",".log",1);

			ps_path << TexPath.Path();
			ps_path.Replace(".tex",".ps",1);
			dvi_path << TexPath.Path();
			dvi_path.Replace(".tex",".dvi",1);
			html_path << parent_dir.Path() << "/" << basename.String() << "/" << basename.String() << ".html";

			BEntry log_entry(log_path.String());
			BEntry pdf_entry(pdf_path.String());
			BEntry ps_entry(ps_path.String());
			BEntry dvi_entry(dvi_path.String());
			BEntry html_entry(html_path.String());

			DoesPdfExist = pdf_entry.Exists();
			DoesPsExist = ps_entry.Exists();
			DoesDviExist = dvi_entry.Exists();
			DoesHtmlExist = html_entry.Exists();
			DoesLogExist = log_entry.Exists();
		}
		//Main Toolbar Buttons

		m_toolBar->TBPrint->SetEnabled(IsNonZeroText);
		m_toolBar->TBSave->SetEnabled(IsSaveNeeded);

		m_toolBar->TBViewLog->SetEnabled(DoesLogExist);
		m_toolBar->TBTexDvi->SetEnabled(DoesTexExist);
		m_toolBar->TBDviPdf->SetEnabled(DoesDviExist);
		m_toolBar->TBDviPs->SetEnabled(DoesDviExist);
		m_toolBar->TBPsPdf->SetEnabled(DoesPsExist);
		m_toolBar->TBTexDvi->SetEnabled(DoesTexExist);
		m_toolBar->TBTexPdf->SetEnabled(DoesTexExist);
		m_toolBar->TBTexHtml->SetEnabled(DoesTexExist);

		m_toolBar->TBOpenTracker->SetEnabled(DoesTexExist);
		m_toolBar->TBOpenTerminal->SetEnabled(DoesTexExist);
		m_toolBar->TBPrevPDF->SetEnabled(DoesPdfExist);
		m_toolBar->TBPrevPS->SetEnabled(DoesPsExist);
		m_toolBar->TBPrevHTML->SetE